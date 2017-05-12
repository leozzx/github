#include <iostream>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <utility>
#include <queue>
#include <thread>

extern thread_local size_t this_thread_id; 
inline size_t get_thread_id() { return this_thread_id; }

class ThreadPool {
public:
  explicit ThreadPool(size_t size);
  ~ThreadPool();

  // Add new work item to the pool.
  template<class F>
  bool enqueue(F f) {
    if (stopped_) return false;

    {
      std::unique_lock<std::mutex> lock(mutex_);
      ++task_size_;
      tasks_.push(std::function<void()>(f));
    }
    task_wait_.notify_one();
    return true;
  }

  template <class Fn, class... Args>
  bool enqueue(Fn&& fn, Args&&... args) {
    return enqueue(std::bind(fn, std::forward<Args>(args)...));
  }

  void start();
  void stop();
  void wait();

private:
  void run(size_t);
  void join_all();

  std::vector<std::thread> workers_;
  std::queue<std::function<void()> > tasks_;
  std::condition_variable task_wait_;
  std::condition_variable check_wait_;
  std::mutex mutex_;
  bool stopped_;
  int32_t task_size_;
  size_t pool_size_;
};
