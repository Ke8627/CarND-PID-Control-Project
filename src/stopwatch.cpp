#include "stopwatch.h"

#include <stdexcept>

Stopwatch::Stopwatch()
  : m_started(false),
    m_elapsedSeconds(0.0)
{
}

double Stopwatch::GetElapsedSeconds() const
{
  return m_elapsedSeconds;
}

void Stopwatch::Start()
{
  if (m_started)
  {
    throw std::runtime_error("Already started.");
  }

  m_started = true;

  m_start = std::chrono::system_clock::now();
}

void Stopwatch::Stop()
{
  if (!m_started)
  {
    throw std::runtime_error("Not started.");
  }

  timestamp current_time = std::chrono::system_clock::now();

  std::chrono::duration<double> duration = current_time - m_start;

  m_elapsedSeconds += duration.count();

  m_started = false;
}

void Stopwatch::Reset()
{
  m_started = false;
  m_elapsedSeconds = 0.0;
}
