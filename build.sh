#! /usr/bin/env sh
g++ main.cc imu.cc i2c.cc -I/usr/local/include/opencv4 -o trackit -L/usr/local/lib -luvc -li2c -lopencv_core -lopencv_highgui -lopencv_stitching -lopencv_imgcodecs -lopencv_features2d
