#include <stdint.h>
#include <string>

/*
 * A utility class to easily record time.
 * It provides start, pause, stop, reset, restart and print function.
 */
class Timer {
  public:
    Timer(bool start = false);
    // get the time in millionseconds
    uint64_t get_timer_ms();
    std::string get_timer_ms_str();
    // get the time in seconds
    uint64_t get_timer_sec();
    std::string get_timer_sec_str();
    // get the time in micro seconds
    uint64_t get_timer();

    // resume after pause
    void resume();
    // reset and start
    void restart();
    void pause();
  private:
    uint64_t get_current_time();

    // store in micro seconds
    uint64_t start_time;
    uint64_t saved_time;
};
