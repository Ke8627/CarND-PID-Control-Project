#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <chrono>

class Stopwatch
{
public:
  Stopwatch();

  double GetElapsedSeconds() const;

  void Start();

  void Stop();

  void Reset();

private:
  typedef std::chrono::time_point<std::chrono::system_clock> timestamp;

  double SecondsSinceStart() const;

  timestamp m_start;

  bool m_started;

  double m_elapsedSeconds;
};
#endif /* TWIDDLE_H */
