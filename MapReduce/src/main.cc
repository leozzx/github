#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <vector>         // std::vector
#include <deque>         // std::deque

/*
template <class Fn, class... Args>
class thread_pool {
  private:
    struct Task {
      Fn&& fn;
      Args&&... args;
      Task(Fn&& f, Args&&... a):
        fn(f), args(a);
    };
    struct Worker {

    };
    std::vector<std::thread> threads;
    std::deque<task> tasks;
    int pool_size;
    void run () {

    }

  public:
    // default constructor
    thread_pool() : pool_size(5);
    // constructor with pool size
    thread_pool(int num) : pool_size(num);
    // add task to pool
    void add_task(Fn&& fn, Args&&... args) {
      tasks.emplace_back(Task(fn, args));
    }
};
*/
#include <iostream>
#include <boost/thread/thread.hpp>
//#include <boost/bind.hpp>
//#include <boost/asio.hpp>

int main () {
  std::cout << "a" << std::endl;
}
