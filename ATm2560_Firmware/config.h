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
#define READ_MODE_CONFIGFILE 0
#define READ_MODE_TEXTREPLACEMENT 1


// config Container
class configuration{
/*********************************** config erzeugen *********************************/
public:
	configuration(void);
	~configuration();
	int write(const char filename[]);
	int read( const char* folder, const char* filename, int read_mode, char* search_string);
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
	int parse_uint8_t(char* buffer,int i,uint8_t* wert);
	int parse_bool(char* buffer,int i,bool* wert);
	int parse_a(char* buffer,int i,char* wert,int max_length);
	int parse_ul(char buffer[],int i,unsigned long *wert);
	int parse_textreplacement(char* buffer, char* search_recopy_string);
	int last_speed_value;
	char hw_version;
};
extern configuration *pConfig;
///////////// CONFIG /////////////////////////

#endif /* CONFIG_H_ */
