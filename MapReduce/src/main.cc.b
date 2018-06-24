#include <iostream>
#include <mutex>
#include <stdio.h>
#include <errno.h>
#include <map>
#include <unordered_map>
#include "thread_pool.hpp"
#include "filesystem.hpp"
#include "hashmap.hpp"
#include "concurrent_hashmap.hpp"
#include "hashmap2.hpp"
#include "timer.hpp"

template<typename ARRAY_t>
void print_array(std::ostream& os, ARRAY_t array, size_t len) {
  os << "[";
  for (size_t i = 0; i < len; ++i) {
    os << array[i] << " ";
  }
  os << "]" << std::endl;
}

//template<typename T>
void write_to_array(int& result, int array[]) {
  auto tid = this_thread_id;
  if (tid == -1) {
    // main thread should not call
    return;
  }
  array[tid] += result;
}

void read_file_and_sum(const char* filename, int array[]) {
  FILE *fp;
  fp = fopen(filename, "r");
  if (fp == NULL) {
    std::cout << "open file error: " << errno << std::endl;
    return;
  }
  int num, sum = 0;
  while (fscanf(fp, "%d", &num) == 1) {
    sum += num;
  }

  fclose(fp);
  write_to_array(sum, array);
}

void test(const char* a, int arr[]) {
  std::cout << a << std::endl;
}

int main () {

  HashMap<int,int> map;
  std::cout << map << std::endl;
  std::cout << "map.count:" << map.count() << std::endl;
  map.put(1,2);
  std::cout << "map.count:" << map.count() << std::endl;
  map.put(1,3);
  std::cout << "map.count:" << map.count() << std::endl;
  map.put(2,2);
  std::cout << "map.count:" << map.count() << std::endl;
  map.put(3,2);
  std::cout << "map.count:" << map.count() << std::endl;
  map.put(3,4);
  std::cout << "map.count:" << map.count() << std::endl;
  std::cout << map << std::endl;
  std::cout << *map.get(1) << " " << *map.get(2) << " " << *map.get(3) << std::endl;
  std::cout << "map.count:" << map.count() << std::endl;
  map.rehash(1);
  std::cout << "map.count:" << map.count() << std::endl;
  std::cout << map << std::endl;
  map.rehash(2);
  std::cout << map << std::endl;
  map.rehash(3);
  std::cout << map << std::endl;
  map.rehash(9);
  std::cout << map << std::endl;
  map.clear();
  std::cout << map << std::endl;



// ------------------------------------------------------


  size_t size = 4;

  // Create a thread pool of 4 worker threads.
  ThreadPool pool(size);

  // the container
  int array[4] = {0,0,0,0};
  std::cout << "initial array ";
  print_array(std::cout, array, size);

  std::vector<std::string> files;
  files_in_dir(files, "/Users/zixuan/git/MapReduce/test");
  print_array(std::cout, files, files.size());

  // map stage
  for (size_t i = 0; i < files.size(); ++i) {
    pool.enqueue(read_file_and_sum, files[i].c_str(), array);
  }


  pool.wait();
  pool.stop();
  
  std::cout << "result array ";
  print_array(std::cout, array, size);

  // reduce stage
  int result = 0;
  for (size_t i = 0; i < size; ++i) {
    result += array[i];
  }

  std::cout << "result " << result << std::endl;


  /*
  std::mutex m;
  std::condition_variable condition_;
  {
    std::unique_lock<std::mutex> lock1(m);
    condition_.notify_all();
    condition_.wait(lock1);
  }
  */
// ------------------------------------------------------

  HashMap<std::string,int> hashmap;
  concurrenthashmap::ConcurrentHashMap<std::string,int> chashmap;
  HashMap2<std::string,int> hashmap2;
  //HashMap<int,int> hashmap;
  std::unordered_map<std::string,int> unorderedmap;
  //std::unordered_map<int,int> unorderedmap;
  std::map<std::string,int> normalmap;
  //std::map<int,int> normalmap;

  Timer timer;

  int count = 100000;
  char str[20];

  // the hash map
  timer.restart();
  for (int i = -count; i <= count; ++i) {
    sprintf(str, "%d", i);
    hashmap.put(std::string(str), i);
  }
  std::cout << "hashmap: " << timer.get_timer_ms_str() << std::endl;

  timer.restart();
  for (int i = -count; i <= count; ++i) {
    sprintf(str, "%d", i);
    chashmap.put(std::string(str), i);
  }
  std::cout << "c hashmap: " << timer.get_timer_ms_str() << std::endl;

  // the hash map 2
  timer.restart();
  for (int i = -count; i <= count; ++i) {
  //for (int i = -count; i <= -99930; ++i) {
    sprintf(str, "%d", i);
  //std::cout << "hashmap2:  " << i << std::endl;
    hashmap2.put(std::string(str), i);
  }
  std::cout << "hashmap2: " << timer.get_timer_ms_str() << std::endl;

  // the unordered map
  timer.restart();
  for (int i = -count; i <= count; ++i) {
    sprintf(str, "%d", i);
    //unorderedmap.put(i, i);
    unorderedmap.insert (std::make_pair<std::string,int>(std::string(str),int(i)));
  }
  std::cout << "unorderedmap: " << timer.get_timer_ms_str() << std::endl;

  // the ordered map
  timer.restart();
  for (int i = -count; i <= count; ++i) {
    sprintf(str, "%d", i);
    //normalmap.put(i, i);
    normalmap.insert (std::make_pair<std::string,int>(std::string(str),int(i)));
  }
  std::cout << "orderedmap: " << timer.get_timer_ms_str() << std::endl;

  std::cout 
    << "size: " << hashmap.count() 
    << " " << chashmap.count() 
    << " " << hashmap2.count() 
    << " " << unorderedmap.size() 
    << " " << normalmap.size() << std::endl;


  return 0;
}
