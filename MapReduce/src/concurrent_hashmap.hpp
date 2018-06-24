#include <iostream>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <memory>
#include <MurmurHash2.h>

#ifndef _CONCURRENT_HASH_MAP_HPP_
#define _CONCURRENT_HASH_MAP_HPP_

namespace concurrenthashmap {

// initial array size
#define INITIAL_BUCKET_COUNT 16
#define MAX_LINKEDLIST_LENGHT 10
#define LOAD_FACTOR 0.75
#define REHASH_FACTOR 2
// more lock better perf and more memory
// 0 1 2 4 ...
#define BUCKET_LOCK_RATIO 0

class SpinLock;

typedef std::mutex mutex_t;
typedef std::unique_lock<mutex_t> lock_t;
typedef std::unique_lock<SpinLock> spinlock_t;
typedef std::condition_variable ConditionVariable_t;
 
class SpinLock {
  public:
    void lock() {
      while(lck.test_and_set(std::memory_order_acquire));
    }
    void unlock() {
      lck.clear(std::memory_order_release);
    }
  private:
    std::atomic_flag lck = ATOMIC_FLAG_INIT;
};

template<class T>
struct KeyHash {
  size_t operator()(const T& k) const {
    return (size_t)MurmurHash64A((void*)&k, sizeof(k), 123);
  }
};

template <class Key, class Value>
class LinkedList {
  public:
    // constructor
    LinkedList(const Key& k, const Value& v) {
      key = k;
      value = v;
      next = nullptr;
    }

    friend std::ostream& operator<< (std::ostream& os, LinkedList& head) {
      LinkedList* p = &head;
      while (p != nullptr) {
        os << " {" << p->key << "," << p->value << "}";
        p = p->next;
      }
      return os;
    }

    Key key;
    Value value;
    LinkedList* next;
};

template <class Key, class Value,
          class HashFcn = KeyHash<Key>,
          class EqualKey = std::equal_to<Key>,
          class Alloc = std::allocator<LinkedList<Key, Value> > 
          //class Alloc = libc_allocator_with_realloc<std::pair<const Key, Value> > 
          >
class ConcurrentHashMap {
  public:
    ConcurrentHashMap() {
      set_bucket_count(INITIAL_BUCKET_COUNT);
      array = new array_t*[bucket_count]();
      locks = new SpinLock[bucket_count >> BUCKET_LOCK_RATIO]();
      size = 0;
      rehash_flag.store(false);
    }

    ~ConcurrentHashMap() {
      if (array != nullptr) {
        clear();
        delete[] array;
        array = nullptr;
      }
      if (locks != nullptr) {
        delete[] locks;
        locks = nullptr;
      }
      size = 0;
    }

    /*
     * put a new key-value pair to the map
     * return true if there is existing value of the key
     */
    inline bool put(const Key& key, const Value& value) {
      // help to rehash first
      check_help();
      return put_internal(key, value, nullptr, true);
    }

    Value* get(const Key& key) {
      // help to rehash first
      check_help();

      // TODO: need a read lock

      Value* result = nullptr;
      size_t idx = hash(key);

      auto p = array[idx];
      while (p != nullptr) {
        if (key_equals(p->key, key)) {
          result = &p->value;
          break;
        }
        p = p->next;
      }

      return result;
    }

    /*
     * Rehash the map.
     * @param count the minimum number of buckets
     * If the count is greater than current number of buckets
     * a rehash is forced.
     * If the count not greater than current number of buckets
     * the function may have no effect.
     */
    void rehash (uint64_t count) {
      // help existing rehash first
      check_help();

      lock_t lck(rehashmutex);
      if (count > bucket_count || (float)size/count< LOAD_FACTOR) {
        rehash_internal(count);
      }
    }

    void clear() {
      // help existing rehash first
      check_help();

      lock_t lck(rehashmutex);

      size = 0;
      for (uint64_t i = 0; i < bucket_count; ++i) {
        spinlock_t lck(locks[i >> BUCKET_LOCK_RATIO]);
        auto p = array[i];
        array[i] = nullptr;
        while (p != nullptr) {
          auto next = p->next;
          p->next = nullptr;
          delete p;
          p = next;
        }
      }
    }

    size_t count() {
      return (size_t)size;
    }

    /*
    Value& operator[](const Key& key) {
    }
    */

    friend std::ostream& operator<< (std::ostream& os, ConcurrentHashMap& map) {
      os << "-------- start of map ---------\n";
      for (uint64_t i = 0; i < map.bucket_count; ++i) {
        os << "  " << i << ": [";
        if (map.array[i] != nullptr)
          os << *map.array[i];
        os << " ]\n";
      }
      os << "-------- end of map ---------";
      return os;
    }

  private:
    typedef LinkedList<Key, Value> array_t;

    // get the hash value of the key
    inline size_t hash(const Key& k) {
      return hash_func(k) & (bucket_count - 1);
    }

    inline bool key_equals(const Key& k1, const Key& k2) {
      return equal_func(k1, k2);
    }

    // help existing rehash
    inline void check_help() {
      if (rehash_flag.load()) {
        rehash_help();
        // busy wait, should not be long
        while(rehash_flag.load());
      }
    }

    bool put_internal(const Key& key, const Value& value,
        array_t* new_node, bool update_size) {

      bool ret = false;
      size_t idx = hash(key);
      // the length of the linkedlist
      size_t length = 0;

      {
        spinlock_t lck(locks[idx >> BUCKET_LOCK_RATIO]);
        if (array[idx] == nullptr) {
          array[idx] = (new_node == nullptr)? new array_t(key, value) : new_node;
          length = 1;
        } else {
          auto p = array[idx];
          while (true) {
            ++length;
            // p can't be nullptr since we check p->next in last iteration
            if (key_equals(p->key, key)) {
              // find the equals key
              // update the value
              p->value = value;
              ret = true;
              break;
            } else if (p->next == nullptr) {
              p->next = (new_node == nullptr)? new array_t(key, value) : new_node;
              break;
            } else {
              // continue find next one
              p = p->next;
            }
          }//end while
        }
      }

      // update size and check rehash condition
      if (update_size) {
        if (!ret) ++size;
        /*
         * 1. if the newly added element causes there are more than
         * MAX_LINKEDLIST_LENGHT elements in one bucket
         * 2. or the size of the map after adding the new element
         * against the number of buckets beyond LOAD_FACTOR
         *
         * we will force a rehash
         */
        lock_t lck(rehashmutex);
        if (length > MAX_LINKEDLIST_LENGHT || (float)size/bucket_count > LOAD_FACTOR) {
          rehash_internal(bucket_count * REHASH_FACTOR);
        }
      }

      return ret;
    }

    void rehash_internal(uint64_t count) {
      rehash_flag.store(true);

      old_count = bucket_count;
      bucket_count = count;
      old_array = array;
      old_locks = locks;
      auto tmp = new array_t*[bucket_count]();
      auto tmp2 = new SpinLock[bucket_count >> BUCKET_LOCK_RATIO]();
      array = tmp;
      locks = tmp2;
      __sync_synchronize();
      rehash_help();
      rehash_index = (1 << bucket_count) - 1;
      // delete old array
      delete[] old_array;
      delete[] old_locks;
      rehash_flag.store(false);
    }

    void rehash_help() {
      // copy old values
      while(true) {
        int i = find_and_set_rehash_index();
        if (i == -1) break;

        spinlock_t lck(old_locks[i >> BUCKET_LOCK_RATIO]);
        auto p = old_array[i];
        while (p != nullptr) {
      //std::cout << "rehash " << p->key << "," << p->value << std::endl;
          auto next = p->next;
          p->next = nullptr;
          put_internal(p->key, p->value, p, false);
          p = next;
        }
      }//end for
    }

    void set_bucket_count(uint64_t new_count) {
      bucket_count = new_count;
      rehash_index = (1 << bucket_count) - 1;
    }
    
    int find_and_set_rehash_index() {
      spinlock_t lck (bitlock);
      /* __builtin_ffsll returns one plus the index of 
       * the least significant 1-bit of x,
       * or if x is zero, returns zero.
       */
      int idx = __builtin_ffsll(rehash_index) - 1;
      if (idx != -1) {
        rehash_index = rehash_index ^ (1 << idx);
      }

      return idx;
    }

    // the array of the key-value pairs
    array_t** array;
    array_t** old_array; // used for rehash
    // the lock array
    SpinLock* locks;
    SpinLock* old_locks;
    // the total size of the map
    uint64_t size;
    // the bucket count of the map
    uint64_t bucket_count;
    uint64_t old_count; // used for rehash
    // the hash function
    HashFcn hash_func;
    // the key comparator
    EqualKey equal_func;
    // bitset for multithread rehash
    uint64_t rehash_index;
    // bitset manipulation lock
    SpinLock bitlock;
    // flag indicate rehashing
    mutex_t rehashmutex;
    std::atomic<bool> rehash_flag;

};

}

#endif /* _CONCURRENT_HASH_MAP_HPP_ */
