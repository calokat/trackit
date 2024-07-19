#ifndef I2C_H
#define I2C_H
#include <cstdint>

class I2C {
public:
	I2C(int address = 0x69);
	bool WriteByte(int address, unsigned char commandCode, unsigned short value);
	bool ReadByte(int address, unsigned char commandCode, int& readValue);
	bool ReadBlock(int address, unsigned char commandCode, int numBytes, uint8_t* values);
private:
	int file;
};
#endif
