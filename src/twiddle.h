#ifndef TWIDDLE_H
#define TWIDDLe_H

#include "PID.h"
#include <iostream>
#include <vector>

class Twiddle
{
public: 
  Twiddle(PID& pid, const std::vector<double>& p, const std::vector<double>& dp);

  void NextRun();

private:
  typedef std::chrono::time_point<std::chrono::system_clock> timestamp;

  void AdvanceParameter();

  void PrintStatus(std::ostream& os, double err);

  PID& m_pid;
  std::vector<double> m_p;
  std::vector<double> m_dp;
  double m_best_err;

  int m_iteration;
  int m_p_index;
  bool m_tried_negative;

  timestamp m_start;
};

#endif /* TWIDDLE_H */
