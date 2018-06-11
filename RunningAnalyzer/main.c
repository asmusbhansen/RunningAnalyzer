/*----------------------------------------------------------------------------/
/  RunningAnalyzer			                              /
/-----------------------------------------------------------------------------/
/
/ Samples accelerometer and gyroscope while storeing the data on a SD Card
/
/----------------------------------------------------------------------------*/ 

#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>

#include "UART.h"
#include "MPU6050.h"
#include "ff.h"
#include "diskio.h"
#include "SDCard.h"

#define F_CPU 16000000UL
#define TIMER1_WAIT 1250

//#define UART_DEBUG 1

int main(void)
{
	uint8_t ret;
	uint8_t file_number = 0;
	char buffer[8];
	char string_buffer[32];
	char file_name[16] = "";
	int sample_counter = 0;
	int timing_counter = 0;
	int bytes_written = 0;
	FATFS FatFs;
	FIL fil;
	FILINFO finfo;
	FRESULT fr;	
	
	struct MPU6050 sensor;

#ifdef UART_DEBUG
	USART_Init();
	USART_Transmit_String("UART Initialized\n\r");
#endif

	i2c_init();

#ifdef UART_DEBUG
	USART_Transmit_String("I2C Initialized\n\r");
#endif

	//Init MPU6050
	ret = MPU6050_init(&sensor);
	if(ret)
	{
#ifdef UART_DEBUG
		USART_Transmit_String("Failed MPU Init\n\r");
#endif
		return 1;
	}
	
	//Test Connection
	ret = MPU6050_Test_Connection();
		
	//Wait for button press
	while((PIND & (1<<PD2)));

	_delay_ms(5000);

	Start_Timer_SD();
	
	//Write file open file
	fr = f_mount(&FatFs, "", 1);
	if(fr == 0)
	{
#ifdef UART_DEBUG
		USART_Transmit_String("Mount succeeded\n\r");
#endif
	}
	else
	{
#ifdef UART_DEBUG
		USART_Transmit_String("Mount failed\n\r");
#endif
	}
	
	while(!f_stat(file_name, &finfo) || strlen(file_name) == 0)
	{
		file_number++;
		sprintf(file_name, "run%d.csv", file_number);
#ifdef UART_DEBUG
		USART_Transmit_String("File name:");
		USART_Transmit_String(file_name);
		USART_Transmit_String("\n\r");
#endif
	}


	fr = f_open(&fil, file_name, FA_CREATE_ALWAYS | FA_WRITE);
	if(fr != FR_OK)
	{
#ifdef UART_DEBUG
		USART_Transmit_String("Log cannot open\n\r");
		USART_Transmit_String("Error code: ");
		USART_Transmit_Integer(fr);
#endif
		f_mount(0, "", 0);
		return fr;
	}
	else
	{
#ifdef UART_DEBUG
		USART_Transmit_String("Log file open\n\r");
#endif
	}
		
			
	Stop_Timer_SD();
	
	//Start timer for sample frequency control
	Start_Timer_Main();
	
#ifdef UART_DEBUG
	USART_Transmit_String("Starting sample loop\n\r");
#endif
	
	_delay_ms(5000);
	
	//While button not pressed
	PORTD |= (1<< PD5);
	
	//Double SPI speed
	SPSR |= (1<<SPI2X);
	
	while((PIND & (1<<PD2)))
	{
		//Reset timer
		TCNT1 = 0;
		
		sample_counter++;
				
		ret = MPU6050_Read_Sensors(&sensor.sensor_values);
		if(ret)
		{
#ifdef UART_DEBUG
			USART_Transmit_String("Failed reading sensor\n\r");
#endif	
			close_and_unmount(sample_counter, timing_counter, &fil);
			
			return 1;			
		}
		
		sensor_values_to_string(&sensor.sensor_values, string_buffer);
				
		fr = f_write(&fil, (const void*)string_buffer, strlen(string_buffer), &bytes_written);
		if(fr)
		{
#ifdef UART_DEBUG
			USART_Transmit_String("Failed writing to SD card\n\r");
#endif
			close_and_unmount(sample_counter, timing_counter, &fil);
			
			return 1;
		}	
	
		if(TCNT1 > TIMER1_WAIT)
		{
			timing_counter++;
		}
		while(TCNT1 <= TIMER1_WAIT);
		
	}
	
	close_and_unmount(sample_counter, timing_counter, &fil);
	
	_delay_ms(10000);
	
	return 1;
}

void Start_Timer_Main()
{
	//Setup TIMER1 for timing control
	
	// set up timer with prescaler = 64
	TCCR1B |= (1 << CS11) | (1 << CS10);
	//Toggle on compare match off OC1A
	TCCR1A = 0x00;
	
	// initialize counter
	TCNT1 = 0;
}

void sensor_values_to_string(struct MPU6050_Sensor_Values *sensor_values, char *string_buffer)
{
	char buffer[16];
	
	//Acc X
	itoa(sensor_values->accX, string_buffer,16);
	
	strcat(string_buffer, ",");
		
	//Acc Y
	itoa(sensor_values->accY, buffer,16);
		
	strcat(string_buffer, buffer);
		
	strcat(string_buffer, ",");
		
	//Acc Z
	itoa(sensor_values->accZ, buffer,16);
		
	strcat(string_buffer, buffer);
		
	strcat(string_buffer, ",");
		
	//Gyro X
	itoa(sensor_values->gyroX, buffer,16);
		
	strcat(string_buffer, buffer);
		
	strcat(string_buffer, ",");
		
	//Gyro Y
	itoa(sensor_values->gyroY, buffer,16);
		
	strcat(string_buffer, buffer);
		
	strcat(string_buffer, "\n\0");
		
	//Gyro Z is not needed here
	//Gyro Z
	/*
	itoa(sensor.sensor_values->gyroZ, buffer,16);
		
	strcat(string_buffer, buffer);
		
	strcat(string_buffer, "\n\0");
	*/
	
}

void close_and_unmount(int sample_counter, int timing_counter, FIL *fil)
{
	//Turn Off LED
	PORTD &= !(1<<PD5);
	
	//Half SPI speed
	SPSR &= ~(1<<SPI2X);
	
	//Sync and close opened file
	f_sync(fil);

	f_close(fil);
	
	//Unmount volume
	f_mount(0, "", 0);

#ifdef UART_DEBUG
	//Print log status
	USART_Transmit_String("Samples: ");
	USART_Transmit_Integer(sample_counter);
	USART_Transmit_String("Timing fails: ");
	USART_Transmit_Integer(timing_counter);
#endif
	
}


