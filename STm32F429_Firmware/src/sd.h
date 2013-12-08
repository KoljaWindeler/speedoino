/*
 * sh.h
 *
 *  Created on: 02.06.2011
 *      Author: kolja
 */

///////////// store ///////////////////////////////////

//#include      <EEPROM.h>                     // EPROM Libary

#ifndef SH_H_
#define SH_H_

#define SD_EN 4

#include "ff.h"

class sd{
#define       CONFIG_FOLDER "config"
public:
	int open();
	int prepare_bildchen();
	void prefetched_animation(int frame_count);


	sd(void);
	~sd();
	void init();
	void power_up(unsigned char tries);
	void power_down();
	void EEPROM_init();

	int8_t writeString(FIL* f, uint8_t* str);
	int8_t writeString(FIL* f, char* str);
	int8_t append_string(uint8_t* filename,uint8_t* buffer);

	int8_t remove_file(uint8_t* path);

	int16_t get_line_n(int16_t line_nr,uint8_t* filename,uint8_t* buffer,uint16_t max_length);
	int16_t get_line_n(int16_t line_nr,uint8_t* filename,uint8_t* buffer,uint16_t max_length, uint16_t* lines_in_file);

	int get_file_handle(unsigned char *msgBuffer,unsigned char *last_file,FIL *file_handle,uint8_t flags);
	int get_file_handle(unsigned char *pathToFile,FIL *file_handle,uint8_t flags);

	int16_t ls_item(uint8_t* path,uint8_t* name,uint16_t item, uint16_t* size);

	bool sd_failed;
	DIR sd_dir;
	FIL sd_file;
	FATFS FatFs;
};
extern sd SD;

#endif /* SH_H_ */
