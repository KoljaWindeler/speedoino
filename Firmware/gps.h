/*
 * gps.h
 *
 *  Created on: 01.06.2011
 *      Author: kolja
 */

#ifndef GPS_H_
#define GPS_H_

#define       SERIAL_BUFFER_SIZE 75   // größe des char buffers für die seriellen gps daten
/**************** gps *******************/
class speedo_gps{
#define SIMPLE_MARK 1
#define RACING_MARK 2
#define NAVI_FOLDER "navi"
public:
	speedo_gps();
	~speedo_gps();
	void init();
	void clear_vars();
	bool check_vars();
	void recv_data();
	long get_info(unsigned char select);
	void loop();
	void check_flag();
	void SendByte(unsigned char data);
	void SendString(const char Str[]);


	unsigned long calc_dist(unsigned long longitude,unsigned long latitude);
	unsigned long mod(unsigned long zahl,unsigned long teiler);
	int get_order(char char_buffer[]);
	void generate_new_order();
	unsigned long nmea_to_dec(unsigned long nmea);
	int get_logged_points(char* buffer,int i);

	// die letzen 30 infos
	unsigned long gps_lati[30],gps_long[30];
	unsigned int speed;
	float navi_ziel_lati,navi_ziel_long;//=9473208.0;
	int navi_point;
	int winkel; // 0-359
	int gps_count; // aktuelle position in der gespeichert wird
	bool gps_count_up[2];
	unsigned int written_gps_points;
	char navi_ziel_name[14];//= "Helmholtzstr."; //seperate var, muss das länger halten
	int active_file; //
	int note_this_place;                          // damit kann man marker auf die strecke setzen
	int valid;
	bool navi_active;
	bool first_valid_gps;
	int gps_write_status;

private:
	long entf; // die entfernung in metern. 32km könnte zuwenig sein, daher long
	int navi_ziel_rl;//=1;
	// die letzen 30 infos
	unsigned int gps_speed_arr[30],gps_course[30],gps_sats[30],gps_fix[30],gps_special[30];
	long gps_alt[30],gps_date[30],gps_time[30];
	bool inner_circle;
	bool gps_ready1;
	bool gps_ready2;
	int gps_state;
	int ringbuf_counter; // position im ring-empfangs-buffer
	char gps_buffer1[SERIAL_BUFFER_SIZE]; // buffer zum entgegennehmen der seriellen daten
	char gps_buffer2[SERIAL_BUFFER_SIZE]; // buffer zum entgegennehmen der seriellen daten
	void store_to_sd();
	void parse(char linea[SERIAL_BUFFER_SIZE],int datensatz);
	void calc_navi();

};
/**************** gps *******************/


#endif /* GPS_H_ */
