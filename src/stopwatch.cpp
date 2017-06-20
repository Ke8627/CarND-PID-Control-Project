#include "stopwatch.h"

#include <stdexcept>

Stopwatch::Stopwatch()
  : m_started(false),
    m_elapsedSeconds(0.0)
{
}

double Stopwatch::GetElapsedSeconds() const
{
  return m_elapsedSeconds + SecondsSinceStart();
}

void Stopwatch::Start()
{
  if (m_started)
  {
    throw std::runtime_error("Already started.");
  }

  m_start = std::chrono::system_clock::now();

  m_started = true;
}

void Stopwatch::Stop()
{
  if (!m_started)
  {
    throw std::runtime_error("Not started.");
  }

  m_elapsedSeconds += SecondsSinceStart();

  m_started = false;
}

void Stopwatch::Reset()
{
  m_elapsedSeconds = 0.0;

  m_started = false;
}

double Stopwatch::SecondsSinceStart() const
{
  if (m_started)
  {
    timestamp current_time = std::chrono::system_clock::now();

    std::chrono::duration<double> duration = current_time - m_start;

    return duration.count();
  }

  return 0.0;
}
