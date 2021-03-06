#include <iostream>
#include <memory>
#include <MurmurHash2.h>

#ifndef _HASH_MAP_HPP_
#define _HASH_MAP_HPP_

// initial array size
#define INITIAL_BUCKET_COUNT 16
#define MAX_LINKEDLIST_LENGHT 10
#define LOAD_FACTOR 0.75
#define REHASH_FACTOR 2

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
class HashMap {
  public:
    HashMap() {
      bucket_count = INITIAL_BUCKET_COUNT;
      array = new array_t*[bucket_count]();
      size = 0;
    }

    ~HashMap() {
      if (array != nullptr) {
        clear();
        delete[] array;
        array = nullptr;
      }
      size = 0;
    }

    /*
     * put a new key-value pair to the map
     * return true if there is existing value of the key
     */
    inline bool put(const Key& key, const Value& value) {
      return put_internal(key, value, nullptr, true);
    }

    Value* get(const Key& key) {
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
      if (count > bucket_count || (float)size/count< LOAD_FACTOR) {
        rehash_internal(count);
      }
    }

    void clear() {
      size = 0;
      for (uint64_t i = 0; i < bucket_count; ++i) {
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

    friend std::ostream& operator<< (std::ostream& os, HashMap& map) {
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

    bool put_internal(const Key& key, const Value& value,
        array_t* new_node, bool update_size) {

      bool ret = false;
      size_t idx = hash(key);
      // the length of the linkedlist
      size_t length = 0;

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
        if (length > MAX_LINKEDLIST_LENGHT || (float)size/bucket_count > LOAD_FACTOR) {
          rehash_internal(bucket_count * REHASH_FACTOR);
        }
      }

      return ret;
    }

    void rehash_internal(uint64_t count) {
      auto old_count = bucket_count;
      bucket_count = count;
      auto old_array = array;
      array = new array_t*[bucket_count]();
      // copy old values
      for (uint64_t i = 0; i < old_count; ++i) {
        auto p = old_array[i];
        while (p != nullptr) {
      //std::cout << "rehash " << p->key << "," << p->value << std::endl;
          auto next = p->next;
          p->next = nullptr;
          put_internal(p->key, p->value, p, false);
          p = next;
        }
      }//end for

      // delete old array
      delete[] old_array;
    }

    // the array of the key-value pairs
    array_t** array;
    // the total size of the map
    uint64_t size;
    // the bucket count of the map
    uint64_t bucket_count;
    // the hash function
    HashFcn hash_func;
    // the key comparator
    EqualKey equal_func;

};

#endif /* _HASH_MAP_HPP_ */
