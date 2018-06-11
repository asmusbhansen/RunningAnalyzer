/*----------------------------------------------------------------------------/
/  SD Card function implementation				                              /
/-----------------------------------------------------------------------------/
/
/ Functions to create, open and write to files on a FAT32 formatted SD Card.
/
/----------------------------------------------------------------------------*/


#ifndef SDCARD_H_
#define SDCARD_H_

#define F_CPU 16000000UL

void Start_Timer();

void Stop_Timer();

void SPI_MasterInit(void);

uint8_t File_Exists(char *File_Name, FATFS *FatFs);

uint8_t Delete_File(char *File_Name, FATFS *FatFs);

uint8_t Create_File(char *File_Name, FATFS *FatFs);

uint8_t Write_File(char *File_Name, FATFS *FatFs, char *data, uint8_t bytes);

#endif /* SDCARD_H_ */