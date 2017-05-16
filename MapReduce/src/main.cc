#include <iostream>
#include <mutex>
#include <stdio.h>
#include <errno.h>
#include "thread_pool.hpp"
#include "filesystem.hpp"

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
  return 0;
}
