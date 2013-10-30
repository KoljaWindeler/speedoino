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

#include      <SdFat.h>
#include      <SdFatUtil.h>

class speedo_sd{
#define       CONFIG_FOLDER "config"
#define       error(s)   error_P(PSTR(s))
public:
	speedo_sd(void);
	~speedo_sd();
	void init();
	void EEPROM_init();
	int writeString(SdFile& f, char *str);
	void power_up(unsigned char tries);
	void power_down();
	void error_P(const char* str);
	void writeCRLF(SdFile& f);
	bool 		  sd_failed;
	Sd2Card       card;
	SdVolume      volume;
};
extern speedo_sd* pSD;

#endif /* SH_H_ */
