#ifndef IMU_H
#define IMU_H
#include "i2c.h"
#include <cstdint>
const int REG_BANK_SEL = 0x7F;
const int AGB0_REG_PWR_MGMT_1 = 0x06;
const int ICM_20948_Internal_Acc = 1 << 0;
const int ICM_20948_Internal_Gyr = 1 << 1;
const int ICM_20948_Internal_Mst = 1 << 4;
const int ICM_20948_Sample_Mode_Continuous = 0x00;
const int ICM_20948_Sample_Mode_Cycled = 0x01;
const int AGB0_REG_LP_CONFIG = 0x05;
const int AGB2_REG_ACCEL_CONFIG_1 = 0x14;
const int AGB2_REG_GYRO_CONFIG_1 = 0x01;
const int AGB0_REG_ACCEL_XOUT_H = 0x2D;

class IMU {
public:
	bool Begin();
	bool Reset();
	void GetAgmt();
private:
	bool SetBank(int bank);
	bool SetSleeping(bool isSleeping);
	bool SetLowPower(bool isLowPower);
	bool SetSampleMode(int devices, int mode);
	bool SetFullScaleRangeAccel(int mode);
	bool SetFullScaleRangeGyro(int mode);
	bool SetDLPFcfgAccel(int filter);
	bool SetDLPFcfgGyro(int filter);
	bool SetDlpfAccelEnabled(bool enabled);
	bool SetDlpfGyroEnabled(bool enabled);
	bool StartupMagnetometer();
	int16_t ToSignedInt(uint16_t i);
	I2C driver;
	int address = 0x69;
	uint16_t axRaw;
	uint16_t ayRaw;
	uint16_t azRaw;
	uint16_t gxRaw;
	uint16_t gyRaw;
	uint16_t gzRaw;
};
#endif
