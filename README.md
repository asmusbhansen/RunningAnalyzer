# RunningAnalyzer

The aim of this project is to develop a battery powered datalogger which fits onto a running shoe.

This is achieved by sampling an accelerometer and a gyroscope while simultaneously storing the data in a .csv file on a FAT formatted SD card.

An Atmega328 microcontroller is reading the sensor values from a MPU6050 motion sensor which contains both the accelerometer and gyroscope. 
The FatFs library is used for creating new files and writing to them on the SD card.   
