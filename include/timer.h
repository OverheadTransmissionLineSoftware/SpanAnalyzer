// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_SPANANALYZER_TIMER_H_
#define OTLS_SPANANALYZER_TIMER_H_

#include <chrono>

/// \par OVERVIEW
///
/// This class is a high precision timer.
class Timer {
 public:
  /// \brief Constructor.
  Timer();

  /// \brief Destructor.
  ~Timer();

  /// \brief Clears the start/stop times.
  void Clear();

  /// \brief Gets the duration in seconds between the start and stop times.
  /// \return The duration between the start and stop times.
  /// If the timer is running, this function will return 0.
  double Duration() const;

  /// \brief Starts the timer.
  void Start();

  /// \brief Stops the timer.
  void Stop();

 private:
  /// \var is_running_
  ///  An indicator that determines if the timer is running.
  mutable bool is_running_;

  /// \var time_end_
  ///   The end time.
  std::chrono::steady_clock::time_point time_end_;

  /// \var time_start_
  ///   The start time.
  std::chrono::steady_clock::time_point time_start_;
};

#endif  // OTLS_SPANANALYZER_TIMER_H_
