#! /usr/bin/env sh
g++ main.cc imu.cc i2c.cc -o trackit -L/usr/local/lib -li2c
