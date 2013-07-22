/*
 * gps.h
 *
 *  Created on: 01.06.2011
 *      Author: kolja
 */

#ifndef GPS_H_
#define GPS_H_

#define    SERIAL_BUFFER_SIZE 75   // größe des char buffers für die seriellen gps daten
#define		INFO_SAT 6
#define		INFO_VALID 9
#define		INFO_SPEED 5
/**************** gps *******************/
class speedo_gps{
#define STD_MARK 0
#define SIMPLE_MARK 1
#define SECTOR_END_MARK 2
#define LAP_END_MARK 3

#define NAVI_FOLDER "navi"
public:
	speedo_gps();
	~speedo_gps();
	void init();
	int check_vars();
	void recv_data();
	long get_info(unsigned char select);
	void loop();
	void check_flag();
	void SendByte(unsigned char data);
	void SendString(const char Str[]);
	bool get_drive_status();
	void set_gps_mark(int type);

	unsigned long calc_dist(unsigned long longitude,unsigned long latitude);
	unsigned long mod(unsigned long zahl,unsigned long teiler);
	int get_order(char char_buffer[], int* dist);
	void generate_new_order();
	unsigned long nmea_to_dec(unsigned long nmea);
	int get_logged_points(char* buffer,int i);
	void reconfigure();
	bool wait_on_gps();
	void update_rate_1Hz();
	void update_rate_10Hz();

	// die letzen 30 infos
	unsigned long gps_lati[30],gps_long[30];
	unsigned int speed;
	float navi_ziel_lati,navi_ziel_long;//=9473208.0;
	int navi_point;
	int winkel; // 0-359
	int gps_count; // aktuelle position in der gespeichert wird
	unsigned int written_gps_points;
	char navi_ziel_name[14];//= "Helmholtzstr."; //seperate var, muss das länger halten
	int active_file; //
	int valid;
	bool navi_active;
	int gps_write_status;

private:
	long entf; // die entfernung in metern. 32km könnte zuwenig sein, daher long
	int navi_ziel_rl;//=1;
	int note_this_place;                          // damit kann man marker auf die strecke setzen
	// die letzen 30 infos
	unsigned int gps_speed_arr[30],gps_course[30],gps_sats[30],gps_fix[30],gps_special[30];
	unsigned int gps_sats_temp,gps_fix_temp;
	long gps_alt[30],gps_date[30],gps_time[30];
	long gps_alt_temp;
	bool inner_circle;
	bool gps_ready1;
	bool gps_ready2;
	bool first_dataset;
	int gps_state;
	int motion_start;
	int ringbuf_counter; // position im ring-empfangs-buffer
	char gps_buffer1[SERIAL_BUFFER_SIZE]; // buffer zum entgegennehmen der seriellen daten
	char gps_buffer2[SERIAL_BUFFER_SIZE]; // buffer zum entgegennehmen der seriellen daten
	int store_to_sd();
	void parse(char linea[SERIAL_BUFFER_SIZE],int datensatz);
	void calc_navi();
	void set_drive_status(int speed, int ss, int sat, char status);

};
/**************** gps *******************/


#endif /* GPS_H_ */
