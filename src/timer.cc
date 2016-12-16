// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "timer.h"

Timer::Timer() {
  is_running_ = false;
}

Timer::~Timer() {
}

void Timer::Clear() {
  time_start_ = std::chrono::steady_clock::time_point::min();
  time_end_ = std::chrono::steady_clock::time_point::min();
}

double Timer::Duration() const {
  if (is_running_ == true) {
    return 0;
  }

  std::chrono::duration<double> time_span =
      std::chrono::duration_cast<std::chrono::duration<double>>
          (time_end_ - time_start_);
  return time_span.count();
}

void Timer::Start() {
  time_start_ = std::chrono::steady_clock::now();
  is_running_ = true;
}

void Timer::Stop() {
  if (is_running_ == true) {
    time_end_ = std::chrono::steady_clock::now();
    is_running_ = false;
  }
}