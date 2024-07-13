#ifndef CONTROL_H
#define CONTROL_H

constexpr unsigned int TIME_ON = 40 * 60000;  // 40 minutes in milliseconds
constexpr unsigned int TIME_OFF = 20 * 60000; // 20 minutes in milliseconds

enum class MachineMode { POWER_OFF = 1, POWER_ON, AUTO };

struct control {
  unsigned int running;
  unsigned int resting;

  // Default constructor
  control() : running(TIME_ON), resting(TIME_OFF) {}

  // Copy constructor
  control(const control &c) : running(c.running), resting(c.resting) {}

  // Assignment operator
  control &operator=(const control &c) {
    if (this == &c) {
      return *this;
    }
    running = c.running;
    resting = c.resting;
    return *this;
  }
};

struct controlData {
  MachineMode mode;
  control timer;

  // Default constructor
  controlData() : mode(MachineMode::AUTO), timer() {}

  // Copy constructor
  controlData(const controlData &cd) : mode(cd.mode), timer(cd.timer) {}

  // Assignment operator
  controlData &operator=(const controlData &data) {
    if (this == &data) {
      return *this;
    }
    mode = data.mode;
    timer = data.timer; // Fixed assignment to timer instead of cd.c
    return *this;
  }
};

controlData &getControlData();

void setupPumpControl();

#endif // CONTROL_H
