#include <uWS/uWS.h>
#include <iostream>
#include <fstream>
#include "json.hpp"
#include "PID.h"
#include <math.h>
#include "twiddle.h"

// for convenience
using json = nlohmann::json;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
std::string hasData(std::string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != std::string::npos) {
    return "";
  }
  else if (b1 != std::string::npos && b2 != std::string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

namespace
{
  static const double c_topSpeed = 30;
  static const double c_medSpeed = 20;
  static const double c_carefulSpeed = 5;
}

double GetIdealSpeed(double steer_cte, double speed)
{
  auto abs_steer_cte = std::abs(steer_cte);

  if (abs_steer_cte < 0.5)
  {
    return c_topSpeed;
  }
  else if (abs_steer_cte > 3)
  {
    return c_carefulSpeed;
  }
  else
  {
    return c_medSpeed;
  }
}

double CalcSpeedCte(double steer_cte, double speed)
{
  double ideal_speed = GetIdealSpeed(steer_cte, speed);

  return speed - ideal_speed;
}

double Constrain(double value, double min, double max)
{
  if (min > max)
  {
    throw std::runtime_error("Invalid arguments.");
  }
  if (value < min)
  {
    return min;
  }
  else if (value > max)
  {
    return max;
  }
  return value;
}

bool SteeringErrorTooLarge(double steer_cte)
{
  static const double c_threshold = 2.2;

  if (std::abs(steer_cte) > c_threshold)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool CarStalled(double speed)
{
  return speed < 2;
}

bool ReachedSpeed(double speed)
{
  return speed > 10;
}

void SteerCar(PID& pid_steer, Twiddle* twiddle)
{
  uWS::Hub h;

  Stopwatch stopwatch;
  stopwatch.Start();

  PID pid_speed;
  pid_speed.Init(0.03, 0.00001, 0.01);

  bool reached_speed = false;
  int ignore_msg_count = 0;

  h.onMessage([&pid_steer,
               &pid_speed,
               &reached_speed,
               &twiddle,
               &ignore_msg_count,
               &stopwatch]
      (uWS::WebSocket<uWS::SERVER> ws,
       char *data,
       size_t length,
       uWS::OpCode opCode)
  {
    if (ignore_msg_count > 0)
    {
      ignore_msg_count -= 1;
      std::string msg = "42[\"reset\",{}]";
      ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      return;
    }

    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2')
    {
      auto s = hasData(std::string(data).substr(0, length));
      if (s != "") {
        auto j = json::parse(s);
        std::string event = j[0].get<std::string>();
        if (event == "telemetry") {
          // j[1] is the data JSON object
          double steer_cte = std::stod(j[1]["cte"].get<std::string>());
          double speed = std::stod(j[1]["speed"].get<std::string>());
          // double angle = std::stod(j[1]["steering_angle"].get<std::string>());

          pid_steer.UpdateError(steer_cte);

          pid_speed.UpdateError(CalcSpeedCte(steer_cte, speed));

          double steer_value = Constrain(pid_steer.GetCorrection(), -1, 1);

          double throttle = Constrain(pid_speed.GetCorrection(), 0, 1);

          if (twiddle)
          {
            if (!reached_speed && ReachedSpeed(speed))
            {
              reached_speed = true;
            }

            if (SteeringErrorTooLarge(steer_cte)
                || (reached_speed && CarStalled(speed))
                || (stopwatch.GetElapsedSeconds() > 250))
            {
              stopwatch.Reset();
              stopwatch.Start();

              reached_speed = false;
              ignore_msg_count = 10;
              twiddle->NextRun();
              pid_speed.Init(0.03, 0.00001, 0.01);

              std::string msg = "42[\"reset\",{}]";
              ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
            }
          }

          /*
          * Calcuate steering value here, remember the steering value is
          * [-1, 1].
          * NOTE: Feel free to play around with the throttle and speed. Maybe use
          * another PID controller to control the speed!
          */

          // DEBUG
          // std::cout << "CTE: " << steer_cte << " Steering Value: " << steer_value << " Speed: " << speed << " Angle: " << angle << " Throttle: " << throttle << std::endl;

          json msgJson;
          msgJson["steering_angle"] = steer_value;
          msgJson["throttle"] = throttle;
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";
          // std::cout << msg << std::endl;
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }
      } else {
        // Manual driving
        std::string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }
  });

  // We don't need this since we're not using HTTP but if it's removed the program
  // doesn't compile :-(
  h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t, size_t) {
    const std::string s = "<h1>Hello world!</h1>";
    if (req.getUrl().valueLength == 1)
    {
      res->end(s.data(), s.length());
    }
    else
    {
      // i guess this should be done more gracefully?
      res->end(nullptr, 0);
    }
  });

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    // std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port))
  {
    std::cout << "Listening to port " << port << std::endl;
  }
  else
  {
    std::cerr << "Failed to listen to port" << std::endl;
    return;
  }
  h.run();
}

int main(int argc, char *argv[])
{
  // Lowest err of 10.0821.
  std::vector<double> p = { 1.52047, 0.0, 1.0 };

  PID pid_steer;
  pid_steer.Init(p[0], p[1], p[2]);

  bool useTwiddle = (argc == 2 && argv[1][0] == 't');

  if (useTwiddle)
  {
    std::cout << "Using twiddle." << std::endl;

    std::vector<double> dp = { 0.5, 0.5, 1.0 };

    std::ofstream fout("params.txt", std::ofstream::app);

    fout << "c_topSpeed=[" << c_topSpeed << "] c_medSpeed=[" << c_medSpeed << "] c_carefulSpeed=[" << c_carefulSpeed << "]" << std::endl;

    Twiddle twiddle(pid_steer, p, dp, fout);

    SteerCar(pid_steer, &twiddle);
  }
  else
  {
    std::cout << "No twiddle." << std::endl;

    SteerCar(pid_steer, nullptr);
  }
}
