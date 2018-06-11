/*----------------------------------------------------------------------------/
/  MPU6050			                              /
/-----------------------------------------------------------------------------/
/
/ Implementation of some core MPU6050 functions
/
/----------------------------------------------------------------------------*/

#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h>

#include "MPU6050.h"
#include "i2c_master.h"

uint8_t MPU6050_init(struct MPU6050 *sensor)
{
	uint8_t ret = 0;
	uint8_t data = 0;
	char buffer[16];
	
	sensor->gyroX_Offset = 0;
	sensor->gyroY_Offset = 0;
	sensor->gyroZ_Offset = 0;

	
	//Read PWR MGMT Reg
	ret = i2c_readReg(MPU6050_ADDRESS, MPU6050_REG_PWR_MGMT_1, &data, 1);

	if (ret)
	{
		return 1;
	}
	
	//Wake up MPU6050
	data = 0;
	ret = i2c_writeReg(MPU6050_ADDRESS, MPU6050_REG_PWR_MGMT_1, &data, 1);
	
	if (ret)
	{
		return 1;
	}
	
	//Read PWR MGMT Reg
	ret = i2c_readReg(MPU6050_ADDRESS, MPU6050_REG_PWR_MGMT_1, &data, 1);
	//If the return value is non zero, or the data read from the WHO_AM_I register is non zero
	if (ret)
	{
		return 1;
	}

	//In this implementation, gyro offset is not accounted for
	/*
	ret = MPU6050_Gyro_Offset(sensor);
	
	if (ret)
	{
		return 1;
	}
	*/	
	
	//Set acc range to 8g
	data = 0x10;
	ret = i2c_writeReg(MPU6050_ADDRESS, MPU6050_REG_ACCEL_CONFIG, &data, 1);
	if (ret)
	{
		return 1;
	}
	
	return 0;
	
}

uint8_t MPU6050_Test_Connection(void)
{
	uint8_t ret;
	uint8_t data;
	char buffer[16];
	
	ret = i2c_readReg(MPU6050_ADDRESS, MPU6050_REG_WHO_AM_I, &data, 1);
	
	if(ret)
	{
		return 1;
	}	

	if(data == MPU6050_ADDRESS >> 1)
	{
		return 0;
	}
	else
	{
		return 1;
	}

}

uint8_t MPU6050_Read_Sensors(struct MPU6050_Sensor_Values *sensor_values)
{
	uint8_t ret = 0;
	uint8_t data[14];
	uint16_t res;
	
	ret = i2c_readReg(MPU6050_ADDRESS, MPU6050_REG_ACCEL_XOUT_H, data, 14);
	
	if(ret)
	{		
		return 1;
	}
		
	sensor_values->accX = (data[0] << 8 ) + data[1];
	
	sensor_values->accY = (data[2] << 8 ) + data[3];
	
	sensor_values->accZ = (data[4] << 8 ) + data[5];
		
	sensor_values->gyroX = (data[8] << 8 ) + data[9];
	
	sensor_values->gyroY = (data[10] << 8 ) + data[11];
	
	sensor_values->gyroZ = (data[12] << 8 ) + data[13];
	
	return 0;
}

uint8_t MPU6050_Gyro_Offset(struct MPU6050 *sensor)
{
	uint8_t ret = 0;
	uint32_t gyroX_Sum = 0;
	uint32_t gyroY_Sum = 0;
	uint32_t gyroZ_Sum = 0;
	
	for(uint16_t i = 0; i < 256; i++)
	{
		ret = MPU6050_Read_Sensors(&sensor->sensor_values);
		
		if(ret)
		{
			return 1;
		}
		
		gyroX_Sum += sensor->sensor_values.gyroX;
		gyroY_Sum += sensor->sensor_values.gyroY;
		gyroZ_Sum += sensor->sensor_values.gyroZ;
		
	}
		
	sensor->gyroX_Offset = (gyroX_Sum >> 8);
	sensor->gyroY_Offset = (gyroY_Sum >> 8);
	sensor->gyroZ_Offset = (gyroZ_Sum >> 8);	
	
	return 0;
}

