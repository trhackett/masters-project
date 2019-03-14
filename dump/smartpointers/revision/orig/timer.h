/*
    Credits to Professor David Smallberg for this small class.
*/

//========================================================================
// Timer t;                 // create a timer
// t.start();               // start the timer
// double d = t.elapsed();  // milliseconds since timer was last started
//========================================================================

#ifndef TIMER_H
#define TIMER_H

#include <chrono>

class Timer
{
  public:
    Timer() {
        start();
    }

    void start() {
        m_time = std::chrono::high_resolution_clock::now();
    }

    double elapsed() const {   
        std::chrono::duration<double,std::milli> diff =
                          std::chrono::high_resolution_clock::now() - m_time;
        return diff.count();
    }

  private:
    std::chrono::high_resolution_clock::time_point m_time;
};

#endif