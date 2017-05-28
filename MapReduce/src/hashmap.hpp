#include <iostream>
#include <MurmurHash2.h>

#ifndef _HASH_MAP_HPP_
#define _HASH_MAP_HPP_

template <class Key, class T,
          //class HashFcn = SPARSEHASH_HASH<Key>,   // defined in sparseconfig.h
          class EqualKey = std::equal_to<Key>//,
          //class Alloc = libc_allocator_with_realloc<std::pair<const Key, T> > 
          >
class HashMap {
public:
};

#endif /* _HASH_MAP_HPP_ */
