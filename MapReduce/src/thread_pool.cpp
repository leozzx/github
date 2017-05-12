#include "thread_pool.hpp"

ThreadPool::ThreadPool(size_t size) : task_size_(0), pool_size_(size) {
  start();
}

ThreadPool::~ThreadPool() {
  stop();
}

void ThreadPool::start() {
  task_size_ = 0;
  for (size_t i = 0; i < pool_size_; ++i) {
    workers_.emplace_back(std::thread(&ThreadPool::run, this, i));
  }
  stopped_ = false;
}

void ThreadPool::stop() {
  stopped_ = true;
  task_wait_.notify_all();
  join_all();
  task_size_ = 0;
  workers_.clear();
}

void ThreadPool::wait() {
  if (stopped_ || task_size_ == 0 || pool_size_ == 0) {
    return;
  }
  while (!stopped_ && task_size_ != 0) {
    std::unique_lock<std::mutex> lock(mutex_);
    check_wait_.wait(lock);
  }
}

void ThreadPool::join_all() {
  size_t size = workers_.size();
  for (size_t i = 0; i < size; ++i) {
    workers_[i].join();
  }
}

thread_local size_t this_thread_id = -1; 
void ThreadPool::run(size_t thread_id) {
  this_thread_id = thread_id;

  while (!stopped_) {
    std::unique_lock<std::mutex> lock(mutex_);
    // wait utill get notified and tasks not empty
    // condition.wait won't eat cpu; while 'while' does.
    std::cout << "|start to wait " << this_thread_id << "|" << std::endl;
    // the wait function will call lock.unlock() while waiting
    // and call lock.lock() when get notified.
    while (!stopped_ && tasks_.empty()) task_wait_.wait(lock);

    // check status
    if (stopped_) break;
    std::cout << "|start to do task|" << std::endl;

    auto task = tasks_.front();
    tasks_.pop();

    lock.unlock();

    try {
      // execute task
      task();
    } catch (std::exception& e) {
      std::cout << "got exception " << e.what() << std::endl;
    } catch (...) {
      std::cout << "got exception " << std::endl;
    }

    std::cout << "|finish to do task|" << std::endl;
    // task finish, decrese task_size_,
    // and notify if all finished
    lock.lock();
    task_size_--;
    if (task_size_ <= 0) check_wait_.notify_one();

  }//end while
}




