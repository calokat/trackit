#include "imu.h"
#include <iostream>
#include <unistd.h>
bool IMU::SetBank(int bank) {
	if (bank > 3) {
		std::cout << "Invalid bank\n";
		return false;
	}
	bank = ((bank << 4) & 0x30);
	return driver.WriteByte(address, REG_BANK_SEL, bank);
}

int IMU::ReadMag(int reg) {
	return I2CMasterSingleR(MAG_AK09916_I2C_ADDR, reg);
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

bool IMU::IsDataReady() {
	SetBank(0);
	int reg;
	driver.ReadByte(address, AGB0_REG_INT_STATUS_1, reg);
	return (reg & 1) > 0;
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
	int idReg;
	bool idRes = driver.ReadByte(address, AGB0_REG_WHO_AM_I, idReg);
	if (!idRes) {
	}
	if (idReg != 0xEA) {
		exit(1);
	}
	Reset();
	usleep(50);
	SetSleeping(false);
	SetLowPower(false);
	SetSampleMode(ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr, ICM_20948_Sample_Mode_Continuous);
	SetFullScaleRangeAccel(0x03);
	SetFullScaleRangeGyro(0x03);
	
	SetDlpfAccelEnabled(true);
	SetDlpfGyroEnabled(true);
	
	StartupMagnetometer();
	
	return true;
}

bool IMU::I2CMasterReset() {
	SetBank(0);
	int reg;
	bool res = driver.ReadByte(address, AGB0_REG_USER_CTRL, reg);
	if (!res) {
		std::cout << "Bad read from AGB0_REG_USER_CTRL\n";
		return false;
	}
	reg |= 1 << 1;
	
	SetBank(0);
	res = driver.WriteByte(address, AGB0_REG_USER_CTRL, reg);
	if (!res) {
		std::cout << "Bad write to AGB0_REG_USER_CTRL\n";
		return false;
	}
	return true;
}

bool IMU::I2CMasterPassthrough(bool passthrough) {
	SetBank(0);
	int reg;
	driver.ReadByte(address, AGB0_REG_INT_PIN_CONFIG, reg);
	if (passthrough) {
		reg |= 1 << 1;
	} else {
		reg &= ~(1 << 1);
	}
	SetBank(0);
	return driver.WriteByte(address, AGB0_REG_INT_PIN_CONFIG, reg);
}

bool IMU::I2CMasterEnable(bool enable) {
	I2CMasterPassthrough(false);
	
	SetBank(3);
	int reg;
	driver.ReadByte(address, AGB3_REG_I2C_MST_CTRL, reg);
	reg &= ~(0x0F);
	reg |= 0x07;
	reg |= 1 << 4;
	
	SetBank(3);
	driver.WriteByte(address, AGB3_REG_I2C_MST_CTRL, reg);
	
	SetBank(0);
	
	bool res = driver.ReadByte(address, AGB0_REG_USER_CTRL, reg);
	if (!res) {
		std::cout << "Failed to read AGB0_REG_USER_CTRL\n";
		exit(1);
	}
	
	if (enable) {
		reg |= 1 << 5;
	} else {
		reg &= ~(1 << 5);
	}
	
	SetBank(0);
	driver.WriteByte(address, AGB0_REG_USER_CTRL, reg);
	
	return true;
}

int IMU::ICM_20948_i2c_master_slv4_txn(int addr, int reg, int data, bool rw, bool send_reg_addr) {
	int _addr = addr;
	if (rw) {
		addr |= 0x80;
	}
	
	SetBank(3);
	driver.WriteByte(address, AGB3_REG_I2C_SLV4_ADDR, _addr);
	
	SetBank(3);
	driver.WriteByte(address, AGB3_REG_I2C_SLV4_REG, reg);
	
	int ctrl_register_slv4 = 0x00;
	ctrl_register_slv4 |= 1 << 7;
	ctrl_register_slv4 &= ~(1 << 6);
	ctrl_register_slv4 &= ~(0x0F);
	if (send_reg_addr) {
		ctrl_register_slv4 &= ~(1 << 5);
	} else {
		ctrl_register_slv4 |= 1 << 5;
	}
	
	bool txnFailed = false;
	if (!rw) {
		SetBank(3);
		driver.WriteByte(address, AGB3_REG_I2C_SLV4_DO, data);
	}
	
	SetBank(3);
	driver.WriteByte(address, AGB3_REG_I2C_SLV4_CTRL, ctrl_register_slv4);
	
	int MAX_CYCLES = 1000;
	int count = 0;
	bool slave4Done = false;
	int statusReg;
	while (!slave4Done) {
		SetBank(0);
		int res = driver.ReadByte(address, AGB0_REG_I2C_MST_STATUS, statusReg);
		if ((statusReg & (1 << 6)) > 0) {
			slave4Done = true;
		}
		if (count > MAX_CYCLES) {
			slave4Done = true;
		}
		count += 1;
	}
	
	if (statusReg & (1 << 4)) {
		txnFailed = true;
	}
	
	if (count > MAX_CYCLES) {
		txnFailed = true;
	}
	
	if (txnFailed) {
		return -1;
	}
	
	if (rw) {
		SetBank(3);
		int retReg;
		driver.ReadByte(address, AGB3_REG_I2C_SLV4_DI, retReg);
		return retReg;
	}
	
	return 0;
}

int IMU::I2CMasterSingleR(int addr, int reg) {
	return ICM_20948_i2c_master_slv4_txn(addr, reg, 0, true, true);
}

int IMU::I2CMasterSingleW(int addr, int reg, int data) {
	return ICM_20948_i2c_master_slv4_txn(addr, reg, data, false, true);
}

int IMU::WriteMag(int reg, int data) {
	return I2CMasterSingleW(MAG_AK09916_I2C_ADDR, reg, data);
}

int16_t IMU::ToSignedInt(uint16_t i) {
	if (i > 32767) {
		i -= 65536;
	}
	return i;
}

void IMU::GetAgmt() {
	if (!IsDataReady()) {
		return;
	}
	uint8_t buff[23];
	SetBank(0);
	driver.ReadBlock(address, AGB0_REG_ACCEL_XOUT_H, sizeof(buff), buff);
	axRaw = ((buff[0] << 8) | (buff[1] & 0xFF));
	ayRaw = ((buff[2] << 8) | (buff[3] & 0xFF));
	azRaw = ((buff[4] << 8) | (buff[5] & 0xFF));
	
	gxRaw = ((buff[6] << 8) | (buff[7] & 0xFF));
	gyRaw = ((buff[8] << 8) | (buff[9] & 0xFF));
	gzRaw = ((buff[10] << 8) | (buff[11] & 0xFF));
	
	std::cout << "Acceleration: ";
	std::cout << "X: " << axRaw << " Y: " << ayRaw << " Z: " << azRaw << std::endl;

	std::cout << "Gyro: ";
	std::cout << "X: " << gxRaw << " Y: " << gyRaw << " Z: " << gzRaw << std::endl;
}

bool IMU::I2CMasterConfigureSlave(int slave, int addr, int reg, int len, bool rw, bool enable, bool data_only, int grp, int swap) {
	int slv_addr_reg = 0x00;
	int slv_reg_reg = 0x00;
	int slv_ctrl_reg = 0x00;
	
	if (slave == 0) {
		slv_addr_reg = AGB3_REG_I2C_SLV0_ADDR;
		slv_reg_reg = AGB3_REG_I2C_SLV0_REG;
		slv_ctrl_reg = AGB3_REG_I2C_SLV0_CTRL;
	} else if (slave == 1) {
		slv_addr_reg = AGB3_REG_I2C_SLV1_ADDR;
		slv_reg_reg = AGB3_REG_I2C_SLV1_REG;
		slv_ctrl_reg = AGB3_REG_I2C_SLV1_CTRL;
	} else if (slave == 2) {
		slv_addr_reg = AGB3_REG_I2C_SLV2_ADDR;
		slv_reg_reg = AGB3_REG_I2C_SLV2_REG;
		slv_ctrl_reg = AGB3_REG_I2C_SLV2_CTRL;
	} else if (slave == 3) {
		slv_addr_reg = AGB3_REG_I2C_SLV3_ADDR;
		slv_reg_reg = AGB3_REG_I2C_SLV3_REG;
		slv_ctrl_reg = AGB3_REG_I2C_SLV3_CTRL;
	} else {
		return false;
	}
	
	SetBank(3);
	
	int address = addr;
	if (rw) {
		address |= 1 << 7;
	}
	driver.WriteByte(address, slv_addr_reg, address);
	
	int subAddress = reg;
	driver.WriteByte(address, slv_reg_reg, subAddress);
	
	int ctrl_reg_slvX = 0x00;
	ctrl_reg_slvX |= len;
	ctrl_reg_slvX |= (int)enable << 7;
	ctrl_reg_slvX |= (int)swap << 6;
	ctrl_reg_slvX |= (int)data_only << 5;
	ctrl_reg_slvX |= (int)grp << 4;
	return driver.WriteByte(address, slv_ctrl_reg, ctrl_reg_slvX);
}

bool IMU::MagWhoIAm() {
	int whoIAm1 = ReadMag(AK09916_REG_WIA1);
	int whoIAm2 = ReadMag(AK09916_REG_WIA2);
	
	return ((whoIAm1 == (MAG_AK09916_WHO_AM_I >> 8)) and (whoIAm2 == (MAG_AK09916_WHO_AM_I & 0xFF)));
}

bool IMU::StartupMagnetometer() {
		I2CMasterPassthrough(false);
		I2CMasterEnable(true);
		
		int tries = 0;
		int maxTries = 5;
		while (tries < maxTries) {
			if (MagWhoIAm()) {
				break;
			}
			I2CMasterReset();
			tries += 1;
		}
		if (tries == maxTries) {
			return false;
		}
		int mag_reg_ctrl2 = 0x00;
		mag_reg_ctrl2 |= AK09916_mode_cont_100hz;
		WriteMag(AK09916_REG_CNTL2, mag_reg_ctrl2);
		
		return I2CMasterConfigureSlave(0, MAG_AK09916_I2C_ADDR, AK09916_REG_ST1, 9, true, true, false, false, false);
}
