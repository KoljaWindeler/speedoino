/*
 * sh.h
 *
 *  Created on: 02.06.2011
 *      Author: kolja
 */

///////////// store ///////////////////////////////////

#include      <EEPROM.h>                     // EPROM Libary

#ifndef SH_H_
#define SH_H_

#include      <SdFat.h>
#include      <SdFatUtil.h>
#define SD_ACTIVE 4 // digitalPin 4 LOW zum einschalten (?) INPUT floating zum ausschalten

class speedo_sd{
#define       CONFIG_FOLDER "config"
#define       error(s)   error_P(PSTR(s))
public:
	speedo_sd(void);
	~speedo_sd();
	void init();
	void power_on();
	void power_off();
	void EEPROM_init();
	int writeString(SdFile& f, char *str);
	void error_P(const char* str);
	void writeCRLF(SdFile& f);
	bool 		  sd_failed;
	Sd2Card       card;
	SdVolume      volume;
};
extern speedo_sd* pSD;

#endif /* SH_H_ */
