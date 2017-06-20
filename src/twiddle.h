#ifndef TWIDDLE_H
#define TWIDDLE_H

#include "PID.h"
#include "stopwatch.h"
#include <iostream>
#include <vector>

class Twiddle
{
public: 
  Twiddle(PID& pid, const std::vector<double>& p, const std::vector<double>& dp, std::ostream& os);

  void NextRun();

private:
  void AdvanceParameter();

  void PrintStatus(std::ostream&os, double err, double seconds);

  PID& m_pid;
  std::vector<double> m_p;
  std::vector<double> m_dp;
  double m_best_err;

  int m_iteration;
  int m_p_index;
  bool m_tried_negative;

  Stopwatch m_stopwatch;

  std::ostream& m_os;
};

#endif /* TWIDDLE_H */
