/*
 * gps.h
 *
 *  Created on: 01.06.2011
 *      Author: kolja
 */

#ifndef GPS_H_
#define GPS_H_

#define    SERIAL_BUFFER_SIZE 75   // grÃ¶Ãe des uint8_t buffers fÃ¼r die seriellen gps daten
#define		INFO_SAT 6
#define		INFO_VALID 9
#define		INFO_SPEED 5

/**************** gps *******************/
class GPS{
#define STD_MARK 0
#define SIMPLE_MARK 1
#define SECTOR_END_MARK 2
#define LAP_END_MARK 3

#define NAVI_FOLDER "navi"


public:
	GPS();
	~GPS();
	void init();
	int check_vars();

	void recv_data(uint8_t byteGPS);
	void loop();
	void check_flag();
	void SendByte(uint8_t data);
	void SendString(const uint8_t *data);
	void set_gps_mark(int type);
	void generate_new_order();
	void update_rate_1Hz();
	void update_rate_10Hz();
	void set_drive_status(int speed, int ss, int sat, uint8_t status);
	void reconfigure();

	bool get_drive_status();
	bool wait_on_gps();

	int calc_gps_goodies();
	int get_order(uint8_t char_buffer[], int* dist);
	int get_logged_points(uint8_t* buffer,int i,int* nbytes);

	long get_info(uint8_t select);
	unsigned long mod(unsigned long zahl,unsigned long teiler);
	unsigned long nmea_to_dec(unsigned long nmea);

	unsigned long calc_dist(unsigned long longitude,unsigned long latitude);
	unsigned long calc_dist_supported(simple_coordinate A);
	unsigned long calc_dist_supported(simple_coordinate A,bool return_square);
	unsigned long calc_dist_supported(simple_coordinate A,simple_coordinate B,float gps_lati_cos,bool return_square);

	// die letzen 30 infos
	unsigned long gps_lati[30],gps_long[30];
	unsigned int speed;
	float navi_ziel_lati,navi_ziel_long;//=9473208.0;
	int16_t navi_point;
	int winkel; // 0-359
	int gps_count; // aktuelle position in der gespeichert wird
	unsigned int written_gps_points;
	uint8_t navi_ziel_name[14];//= "Helmholtzstr."; //seperate var, muss das lÃ¤nger halten
	int16_t active_file; //
	int valid;
	bool navi_active;
	bool use_compressed_log_format;
	int gps_write_status;
	simple_coordinate gps_goody;

private:
	long entf; // die entfernung in metern. 32km kÃ¶nnte zuwenig sein, daher long
	int navi_ziel_rl;//=1;
	int note_this_place;                          // damit kann man marker auf die strecke setzen
	uint8_t checksum;
	// die letzen 30 infos
	unsigned int gps_speed_arr[30],gps_course[30],gps_sats[30],gps_fix[30],gps_special[30];
	unsigned int gps_sats_temp,gps_fix_temp;
	long gps_alt[30],gps_date[30],gps_time[30];
	unsigned long gps_timestamp;

	float gps_lati_cos;
	long gps_alt_temp;
	bool inner_circle;
	bool gps_ready1;
	bool gps_ready2;
	bool first_dataset;
	int gps_state;
	int motion_start;
	int ringbuf_counter; // position im ring-empfangs-buffer
	uint8_t gps_buffer1[SERIAL_BUFFER_SIZE]; // buffer zum entgegennehmen der seriellen daten
	uint8_t gps_buffer2[SERIAL_BUFFER_SIZE]; // buffer zum entgegennehmen der seriellen daten
	int store_to_sd();
	void parse(uint8_t *linea,int datensatz);
	void calc_navi();
	unsigned long GpsTimeToTimeStamp(unsigned long input);
};
/**************** gps *******************/


#endif /* GPS_H_ */
