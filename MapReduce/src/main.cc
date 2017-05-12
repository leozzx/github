#include <iostream>
#include <mutex>
#include "thread_pool.hpp"

void test(int i) {
      std::cout << "hello " << i << std::endl;
      boost::this_thread::sleep(boost::posix_time::seconds(3));
      std::cout << "world " << i << std::endl;
}

int main () {
  // Create a thread pool of 4 worker threads.
  ThreadPool pool(4);

  // the container
  int array[4] = {0,0,0,4};
  std::cout << "array " << array[3] << std::endl;

  std::mutex mutex;
  // Queue a bunch of work items.
  for (int i = 0; i < 8; ++i) {
    pool.enqueue([&mutex] {
      std::unique_lock<std::mutex> lock(mutex);
      std::cout << "hello " << get_thread_id() << std::endl;
      boost::this_thread::sleep(boost::posix_time::seconds(3));
      std::cout << "world " << get_thread_id() << std::endl;
    });
    pool.enqueue(test, i*2);
  }

  pool.wait();
  pool.stop();
  
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
