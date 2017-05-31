#include <iostream>
#include <memory>
#include <MurmurHash2.h>
#include <stdexcept>

#ifndef _HASH_MAP_HPP_
#define _HASH_MAP_HPP_

// initial array size
#define INITIAL_BUCKET_COUNT 16

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
      next = NULL;
    }

    friend std::ostream& operator<< (std::ostream& os, LinkedList& head) {
      LinkedList* p = &head;
      while (p != NULL) {
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
      if (array != NULL) {
        clear();
        delete array;
        array = NULL;
      }
    }

    /*
     * put a new key-value pair to the map
     * return true if there is existing value of the key
     */
    bool put(const Key& key, const Value& value) {
      bool ret = false;
      size_t idx = hash(key);

      if (array[idx] == NULL) {
        array[idx] = new array_t(key, value);
      } else {
        auto p = array[idx];
        while (true) {
          // p can't be NULL since we check p->next in last iteration
          if (key_equals(p->key, key)) {
            // find the equals key
            // update the value
            p->value = value;
            ret = true;
            break;
          } else if (p->next == NULL) {
            p->next = new array_t(key, value);
            break;
          } else {
            // continue find next one
            p = p->next;
          }
        }//end while
      }

      size++;

      return ret;
    }

    Value* get(const Key& key) {
      Value* result = NULL;
      size_t idx = hash(key);

      auto p = array[idx];
      while (p != NULL) {
        if (key_equals(p->key, key)) {
          result = &p->value;
          break;
        }
        p = p->next;
      }

      return result;
    }

    void rehash (uint64_t count) {
      if (count == 0) {
        throw new std::range_error("rehash function requires count at least 1");
      }

      auto old_count = bucket_count;
      bucket_count = count;
      auto old_array = array;
      array = new array_t*[bucket_count]();
      // copy old values
      for (uint64_t i = 0; i < old_count; ++i) {
        auto p = old_array[i];
        while (p != NULL) {
          put(p->key, p->value);
          p = p->next;
        }
      }//end for
    }

    void clear() {
      for (uint64_t i = 0; i < bucket_count; ++i) {
        auto p = array[i];
        array[i] = NULL;
        while (p != NULL) {
          auto next = p->next;
          p->next = NULL;
          delete p;
          p = next;
        }
      }
    }

    /*
    Value& operator[](const Key& key) {
    }
    */

    friend std::ostream& operator<< (std::ostream& os, HashMap& map) {
      os << "-------- start of map ---------\n";
      for (uint64_t i = 0; i < map.bucket_count; ++i) {
        os << "  " << i << ": [";
        if (map.array[i] != NULL)
          os << *map.array[i];
        os << " ]\n";
      }
      os << "-------- end of map ---------";
      return os;
    }

  private:
    typedef LinkedList<Key, Value> array_t;

    // get the hash value of the key
    size_t hash(const Key& k) {
      return hash_func(k) & (bucket_count - 1);
    }

    bool key_equals(const Key& k1, const Key& k2) {
      return equal_func(k1, k2);
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
