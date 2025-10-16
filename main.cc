#include <unistd.h>
#include <string>

#include "imu.h"

static const std::string WINDOW_NAME = "Hello Trackit!";

int main(int argc, char **argv) {
  IMU imu;
  imu.Begin();
}
