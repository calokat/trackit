#include "i2c.h"
#include "imu.h"
extern "C" {
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>	
}
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

I2C::I2C(int address) {
	file = open("/dev/i2c-1", O_RDWR);
	if (file < 0) {
		std::cout << "Failed to open device\n";
		exit(1);
	}
	if (ioctl(file, I2C_SLAVE, address) < 0) {
		std::cout << "Failed to call ioctl\n";
		exit(1);
	}
}

bool I2C::ReadByte(int address, unsigned char commandCode, int& readValue) {
	readValue = i2c_smbus_read_word_data(file, commandCode);
	return readValue >= 0;
}

bool I2C::WriteByte(int address, unsigned char commandCode, unsigned short value) {
	int result = i2c_smbus_write_word_data(file, commandCode, value);
	return result >= 0;
}

bool I2C::ReadBlock(int address, unsigned char commandCode, int numBytes, uint8_t* values) {
	int numBytesRead = i2c_smbus_read_i2c_block_data(file, AGB0_REG_ACCEL_XOUT_H, numBytes, values);
	return true;
}
