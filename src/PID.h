#ifndef PID_H
#define PID_H

#include <chrono>

class PID
{
public:
  /*
  * Constructor
  */
  PID();

  /*
  * Destructor.
  */
  virtual ~PID();

  /*
  * Initialize PID.
  */
  void Init(double Kp, double Ki, double Kd);

  /*
  * Update the PID error variables given cross track error.
  */
  void UpdateError(double cte);

  double GetCorrection() const;

  /*
  * Calculate the total PID error.
  */
  double TotalError();

private:
  typedef std::chrono::time_point<std::chrono::system_clock> timestamp;
  timestamp prev_time;

  bool has_prev_error;

  /*
  * Errors
  */
  double p_error;
  double i_error;
  double d_error;

  /*
  * Coefficients
  */
  double Kp;
  double Ki;
  double Kd;
};

#endif /* PID_H */
