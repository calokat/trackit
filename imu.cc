#include "imu.h"
#include <iostream>

bool IMU::SetBank(int bank) {
	if (bank > 3) {
		std::cout << "Invalid bank\n";
		return false;
	}
	bank = ((bank << 4) & 0x30);
	return driver.WriteByte(address, REG_BANK_SEL, bank);
}

bool IMU::Reset() {
	SetBank(0);
	int reg = 42;
	int result = driver.ReadByte(address, AGB0_REG_PWR_MGMT_1, reg);
	if (!result) {
		std::cout << "Failed to read power management bit " << reg << std::endl;
		exit(1);
	}
	reg |= (1<<7);
	SetBank(0);
	return driver.WriteByte(address, AGB0_REG_PWR_MGMT_1, reg);
}

bool IMU::SetSleeping(bool isSleeping) {
	SetBank(0);
	int reg;
	int res = driver.ReadByte(address, AGB0_REG_PWR_MGMT_1, reg);
	if (!res) {
		std::cout << "Failed to read power management bit\n";
		exit(1);
	}
	
	if (isSleeping) {
		reg |= 1 << 6;
	} else {
		reg &= ~(1 << 6);
	}
	
	SetBank(0);
	return driver.WriteByte(address, AGB0_REG_PWR_MGMT_1, reg);
}

bool IMU::SetLowPower(bool isLowPower) {
	SetBank(0);
	int reg;
	int result = driver.ReadByte(address, AGB0_REG_PWR_MGMT_1, reg);
	if (isLowPower) {
		reg |= 1 << 5;
	} else {
		reg &= ~(1 << 5);
	}
	
	SetBank(0);
	return driver.WriteByte(address, AGB0_REG_PWR_MGMT_1, reg);
}

bool IMU::SetSampleMode(int devices, int mode) {
	SetBank(0);
	int reg;
	int res = driver.ReadByte(address, AGB0_REG_LP_CONFIG, reg);
	if (devices & ICM_20948_Internal_Acc) {
		if (mode == ICM_20948_Sample_Mode_Cycled) {
			reg |= 1 << 5;
		} else if (mode == ICM_20948_Sample_Mode_Continuous) {
			reg &= ~(1 << 5);
		}
	}
	if (devices & ICM_20948_Internal_Gyr) {
		if (mode == ICM_20948_Sample_Mode_Cycled) {
			reg |= 1 << 4;
		} else if (mode == ICM_20948_Sample_Mode_Continuous) {
			reg &= ~(1 << 4);
		}
	}
	if (devices & ICM_20948_Internal_Mst) {
		if (mode == ICM_20948_Sample_Mode_Cycled) {
			reg |= 1 << 6;
		} else if (mode == ICM_20948_Sample_Mode_Continuous) {
			reg &= ~(1 << 6);
		}
	}
	
	SetBank(0);
	return driver.WriteByte(address, AGB0_REG_LP_CONFIG, reg);
}

bool IMU::SetFullScaleRangeAccel(int mode) {
	SetBank(2);
	int reg;
	int res = driver.ReadByte(address, AGB2_REG_ACCEL_CONFIG_1, reg);
	
	if (!res) {
		std::cout << "SetFullScaleRangeAccel failed\n";
		exit(1);
	}
	
	reg &= ~(0b00000110);
	reg |= mode << 1;
	
	SetBank(2);
	return driver.WriteByte(address, AGB2_REG_ACCEL_CONFIG_1, reg);
}

bool IMU::SetFullScaleRangeGyro(int mode) {
	SetBank(2);
	
	int reg;
	int res = driver.ReadByte(address, AGB2_REG_GYRO_CONFIG_1, reg);
	
	if (!res) {
		std::cout << "SetFullScaleRangeGyro failed\n";
		exit(1);
	}

	reg &= ~(0b00000110);
	
	reg |= mode << 1;
	
	SetBank(2);
	return driver.WriteByte(address, AGB2_REG_GYRO_CONFIG_1, reg);
}

bool IMU::SetDLPFcfgAccel(int filter) {
	SetBank(2);
	
	int reg;
	bool res = driver.ReadByte(address, AGB2_REG_ACCEL_CONFIG_1, reg);
	
	if (!res) {
		std::cout << "SetDLPFcfgAccel failed\n";
		exit(1);
	}
	
	reg &= ~(0b00111000);
	reg |= filter << 3;
	
	SetBank(2);
	return driver.WriteByte(address, AGB2_REG_ACCEL_CONFIG_1, reg);
}

bool IMU::SetDLPFcfgGyro(int filter) {
	SetBank(2);
	
	int reg;
	bool res = driver.ReadByte(address, AGB2_REG_GYRO_CONFIG_1, reg);
	
	if (!res) {
		std::cout << "SetDLPFcfgGyro failed\n";
		exit(1);
	}
	
	reg &= ~(0b00111000);
	reg |= filter << 3;
	
	SetBank(2);
	return driver.WriteByte(address, AGB2_REG_GYRO_CONFIG_1, reg);
}

bool IMU::SetDlpfAccelEnabled(bool enabled) {
	SetBank(2);
	int reg;
	bool res = driver.ReadByte(address, AGB2_REG_ACCEL_CONFIG_1, reg);
	if (enabled) {
		reg |= 1 << 0;
	} else {
		reg &= ~(1 << 0);
	}
	
	SetBank(2);
	return driver.WriteByte(address, AGB2_REG_ACCEL_CONFIG_1, reg);
}

bool IMU::SetDlpfGyroEnabled(bool enabled) {
	SetBank(2);
	int reg;
	bool res = driver.ReadByte(address, AGB2_REG_GYRO_CONFIG_1, reg);
	if (enabled) {
		reg |= 1 << 0;
	} else {
		reg &= ~(1 << 0);
	}
	
	SetBank(2);
	return driver.WriteByte(address, AGB2_REG_GYRO_CONFIG_1, reg);
}

bool IMU::Begin() {
	SetBank(0);
	Reset();
	SetSleeping(false);
	SetLowPower(false);
	SetSampleMode(ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr, ICM_20948_Sample_Mode_Continuous);
	SetFullScaleRangeAccel(3);
	SetFullScaleRangeGyro(3);
	
	SetDlpfAccelEnabled(true);
	SetDlpfGyroEnabled(true);
	
	return true;
}

void IMU::GetAgmt() {
	uint8_t buff[23];
	SetBank(0);
	driver.ReadBlock(address, AGB0_REG_ACCEL_XOUT_H, sizeof(buff), buff);
	axRaw = ((buff[0] << 8) | (buff[1] & 0xFF));
	ayRaw = ((buff[2] << 8) | (buff[3] & 0xFF));
	azRaw = ((buff[4] << 8) | (buff[5] & 0xFF));
	
	std::cout << "Acceleration:\n";
	std::cout << "X: " << axRaw << std::endl;
	std::cout << "Y: " << ayRaw << std::endl;
	std::cout << "Z: " << azRaw << std::endl;
}
