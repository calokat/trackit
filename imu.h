#ifndef IMU_H
#define IMU_H
#include "i2c.h"
#include <cstdint>
const int REG_BANK_SEL = 0x7F;
const int AGB0_REG_PWR_MGMT_1 = 0x06;
const int AGB0_REG_WHO_AM_I = 0x00;
const int ICM_20948_Internal_Acc = 1 << 0;
const int ICM_20948_Internal_Gyr = 1 << 1;
const int ICM_20948_Internal_Mst = 1 << 4;
const int ICM_20948_Sample_Mode_Continuous = 0x00;
const int ICM_20948_Sample_Mode_Cycled = 0x01;
const int AGB0_REG_LP_CONFIG = 0x05;
const int AGB2_REG_ACCEL_CONFIG_1 = 0x14;
const int AGB2_REG_GYRO_CONFIG_1 = 0x01;
const int AGB0_REG_ACCEL_XOUT_H = 0x2D;
const int AGB0_REG_INT_STATUS_1 = 0x1A;
const int AGB0_REG_INT_PIN_CONFIG = 0x0F;
const int AGB3_REG_I2C_MST_CTRL = 0x01;
const int AGB0_REG_USER_CTRL = 0x03;
const int AGB3_REG_I2C_SLV4_ADDR = 0x13;
const int AGB3_REG_I2C_SLV4_REG = 0x14;
const int AGB3_REG_I2C_SLV4_DO = 0x16;
const int AGB3_REG_I2C_SLV4_CTRL = 0x15;
const int AGB0_REG_I2C_MST_STATUS = 0x17;
const int AGB3_REG_I2C_SLV4_DI = 0x17;
const int MAG_AK09916_I2C_ADDR = 0x0C;
const int AGB3_REG_I2C_SLV0_ADDR = 0x03;
const int AGB3_REG_I2C_SLV0_REG = 0x04;
const int AGB3_REG_I2C_SLV0_CTRL = 0x05;
const int AGB3_REG_I2C_SLV1_ADDR = 0x07;
const int AGB3_REG_I2C_SLV1_REG = 0x08;
const int AGB3_REG_I2C_SLV1_CTRL = 0x09;
const int AGB3_REG_I2C_SLV2_ADDR = 0x0B;
const int AGB3_REG_I2C_SLV2_REG = 0x0C;
const int AGB3_REG_I2C_SLV2_CTRL = 0x0D;
const int AGB3_REG_I2C_SLV3_ADDR = 0x0F;
const int AGB3_REG_I2C_SLV3_REG = 0x10;
const int AGB3_REG_I2C_SLV3_CTRL = 0x11;
const int AK09916_mode_cont_100hz = 0x04 << 1;
const int AK09916_REG_WIA1 = 0x00;
const int AK09916_REG_WIA2 = 0x01;
const int AK09916_REG_CNTL2 = 0x31;
const int AK09916_REG_ST1 = 0x10;
const int MAG_AK09916_WHO_AM_I = 0x4809;
class IMU {
public:
	bool Begin();
	bool Reset();
	void GetAgmt();
	bool IsDataReady();
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
	bool I2CMasterPassthrough(bool passthrough);
	bool I2CMasterEnable(bool enable);
	bool I2CMasterReset();
	int ICM_20948_i2c_master_slv4_txn(int addr, int reg, int data, bool rw, bool send_reg_addr);
	int WriteMag(int reg, int data);
	int I2CMasterSingleR(int addr, int reg);
	int I2CMasterSingleW(int addr, int reg, int data);
	bool I2CMasterConfigureSlave(int slave, int addr, int reg, int len, bool rw, bool enable, bool data_only, int grp, int swap);
	int ReadMag(int reg);
	bool MagWhoIAm();
	int16_t ToSignedInt(uint16_t i);
	I2C driver;
	int address = 0x69;
	int16_t axRaw;
	int16_t ayRaw;
	int16_t azRaw;
	int16_t gxRaw;
	int16_t gyRaw;
	int16_t gzRaw;
};
#endif
