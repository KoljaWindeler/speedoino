/*
 * config.h
 *
 *  Created on: 01.06.2011
 *      Author: kolja
 */

#ifndef CONFIG_H_
#define CONFIG_H_
//#include      <EEPROM.h>                     // EPROM Libary
///////////// CONFIG /////////////////////////
/*********************************** config erzeugen *********************************
 * denk dran das zu splitten:
 * 1. hier oben gucken obs i,a oder f ist, dann ne position finden (array erweitern ? )
 * 2. unten noch eine if zweig pro var einbauen und die parser methode whlen
 *********************************** config erzeugen *********************************/
// config Container
class configuration{
/*********************************** config erzeugen *********************************/
public:
	configuration(void);
	~configuration();
	int write(const char filename[]);
	int read(const char filename[]);
	int get_hw_version();
	int read_skin();
	int compare_substr( char *string1, char *string2, int amount );
	void km_save();
	void EEPROM_init();
	unsigned char eeprom_read(int address);
	void day_trip_check();
	void ram_info();
	bool storage_outdated;
	int skin_file;
private:
	int parse(char buffer[]);
	int parse_float(char buffer[],int i,float *wert);
	int parse_int(char buffer[],int i,int *wert);
	short parse_short(char* buffer,int i,short* wert);
	int parse_bool(char* buffer,int i,bool* wert);
	int parse_a(char* buffer,int i,char* wert,int max_length);
	int parse_ul(char buffer[],int i,unsigned long *wert);
	int last_speed_value;
};
extern configuration *pConfig;
///////////// CONFIG /////////////////////////

#endif /* CONFIG_H_ */
