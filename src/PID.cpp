#include "PID.h"

using namespace std;

/*
* Complete the PID class.
*/

PID::PID()
  : has_prev_error(false),
    p_error(0),
    i_error(0),
    d_error(0),
    Kp(0),
    Ki(0),
    Kd(0)
{
}

PID::~PID()
{
}

void PID::Init(double newKp, double newKi, double newKd)
{
  Kp = newKp;
  Ki = newKi;
  Kd = newKd;
}

void PID::UpdateError(double cte)
{
  timestamp current_time = std::chrono::system_clock::now();

  if (has_prev_error)
  {
    std::chrono::duration<double> dt = current_time - prev_time;

    d_error = (cte - p_error) / dt.count();
  }

  p_error = cte;
  i_error += cte;

  prev_time = current_time;

  has_prev_error = true;
}

double PID::GetSteerValue() const
{
  return -Kp * p_error - Ki * i_error - Kd * d_error;
}

double PID::TotalError()
{
  return 0;
}

