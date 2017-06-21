# CarND-Controls-PID
Self-Driving Car Engineer Nanodegree Program

---

## Reflections

### Describe the effect each of the P, I, D components had in your implementation.

- The proportional component (P) steers the car towards the desired trajectory.
- The integral component (I), theoretically, overcomes bias in the steering (e.g., wheels are not perfectly aligned with steering wheel).
  - In my implementation, the car performed acceptably with small or even zero I coefficient.
- The derivative component (D) avoids overshooting the desired trajectory and oscillating around it.
- `demo.m4v` is a video of the car in the simulator with my final hyperparameters described below

### Describe how the final hyperparameters were chosen.

My hyperparameters are:

- PID coefficients for steering
  - (1.52047, 0.0, 1.0)
- PID coefficients for speed
  - (0.03, 0.00001, 0.01)
- Target speeds
  - These are target speeds for the speed PID controller:
    - c_topSpeed = 30
    - c_medSpeed = 20
    - c_carefulSpeed = 5
  - When cross-track error (CTE) is low, then the target speed is set higher
  - When CTE is high, then the car should slow down to get back on track, so the target speed is lowered
  - See `GetIdealSpeed()` in `main.cpp`

This is how I chose the final hyperparameters:

- I initially tuned the steering PID coefficients manually, but found it to be tedious, so I added the `Twiddle` class.
  - `report/params.txt` contains the output from my twiddle experiments
- I defined error using this formula (`Twiddle::NextRun()` in `twiddle.cpp`):
  - `err = TotalError / RunTime + 2000 / RunTime`
  - The first term penalizes cross-track error (i.e., being far from the desired trajectory)
  - The second term penalizes short run time
- Run time is measured in seconds:
  - Start: Run start
  - End:
    - Steering has failed (details below) or
    - Car has run successfully for 250 seconds
- Steering has failed when either of these criteria is met (`onMessage()` lambda in `main.cpp`):
  - The cross-track error is too large
    - `abs(steer_cte) > 2.2`
    - See `SteeringErrorTooLarge()` in `main.cpp`
  - The car has stalled after reaching a minimum speed
    - Minimum speed: 10 mph
    - Car stall threshold: < 2 mph
    - See `CarStalled()` in `main.cpp`
- I tuned the speed PID coefficients and target speeds manually
  - I tried higher target speeds like 60 mph, but my twiddle runs could not find parameters that would reliably complete the track

## Dependencies

* cmake >= 3.5
 * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools]((https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)
* [uWebSockets](https://github.com/uWebSockets/uWebSockets)
  * Run either `./install-mac.sh` or `./install-ubuntu.sh`.
  * If you install from source, checkout to commit `e94b6e1`, i.e.
    ```
    git clone https://github.com/uWebSockets/uWebSockets 
    cd uWebSockets
    git checkout e94b6e1
    ```
    Some function signatures have changed in v0.14.x. See [this PR](https://github.com/udacity/CarND-MPC-Project/pull/3) for more details.
* Simulator. You can download these from the [project intro page](https://github.com/udacity/self-driving-car-sim/releases) in the classroom.

There's an experimental patch for windows in this [PR](https://github.com/udacity/CarND-PID-Control-Project/pull/3)

## Basic Build Instructions

1. Clone this repo.
2. Make a build directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./pid`. 

## Editor Settings

We've purposefully kept editor configuration files out of this repo in order to
keep it as simple and environment agnostic as possible. However, we recommend
using the following settings:

* indent using spaces
* set tab width to 2 spaces (keeps the matrices in source code aligned)

## Code Style

Please (do your best to) stick to [Google's C++ style guide](https://google.github.io/styleguide/cppguide.html).

## Project Instructions and Rubric

Note: regardless of the changes you make, your project must be buildable using
cmake and make!

More information is only accessible by people who are already enrolled in Term 2
of CarND. If you are enrolled, see [the project page](https://classroom.udacity.com/nanodegrees/nd013/parts/40f38239-66b6-46ec-ae68-03afd8a601c8/modules/f1820894-8322-4bb3-81aa-b26b3c6dcbaf/lessons/e8235395-22dd-4b87-88e0-d108c5e5bbf4/concepts/6a4d8d42-6a04-4aa6-b284-1697c0fd6562)
for instructions and the project rubric.

## Hints!

* You don't have to follow this directory structure, but if you do, your work
  will span all of the .cpp files here. Keep an eye out for TODOs.

## Call for IDE Profiles Pull Requests

Help your fellow students!

We decided to create Makefiles with cmake to keep this project as platform
agnostic as possible. Similarly, we omitted IDE profiles in order to we ensure
that students don't feel pressured to use one IDE or another.

However! I'd love to help people get up and running with their IDEs of choice.
If you've created a profile for an IDE that you think other students would
appreciate, we'd love to have you add the requisite profile files and
instructions to ide_profiles/. For example if you wanted to add a VS Code
profile, you'd add:

* /ide_profiles/vscode/.vscode
* /ide_profiles/vscode/README.md

The README should explain what the profile does, how to take advantage of it,
and how to install it.

Frankly, I've never been involved in a project with multiple IDE profiles
before. I believe the best way to handle this would be to keep them out of the
repo root to avoid clutter. My expectation is that most profiles will include
instructions to copy files to a new location to get picked up by the IDE, but
that's just a guess.

One last note here: regardless of the IDE used, every submitted project must
still be compilable with cmake and make./
