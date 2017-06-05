#include "timer.hpp"
#include <time.h>
#include <sys/time.h>
#include <sstream>

Timer::Timer(bool start) {
  start_time = 0;
  if (start) {
    resume();
  }
  saved_time = 0;
}

uint64_t Timer::get_timer() {
  auto current_time = start_time == 0? 0 : get_current_time();
  return current_time - start_time + saved_time;
}

uint64_t Timer::get_timer_ms() {
  return get_timer()/1000;
}

std::string Timer::get_timer_ms_str() {
  std::stringstream ss;
  ss << get_timer_ms() << "ms";
  return ss.str();
}

uint64_t Timer::get_timer_sec() {
  return get_timer_ms()/1000;
}

std::string Timer::get_timer_sec_str() {
  std::stringstream ss;
  ss << get_timer_sec() << "ms";
  return ss.str();
}

uint64_t Timer::get_current_time() {
  struct timeval tv;
  gettimeofday(&tv, 0);
  return (uint64_t)tv.tv_sec * 1000000L + tv.tv_usec;
}

void Timer::pause() {
  saved_time += get_timer();
  start_time = 0;
}

void Timer::resume() {
  start_time = get_current_time();
}

void Timer::restart() {
  start_time = 0;
  saved_time = 0;
  resume();
}

