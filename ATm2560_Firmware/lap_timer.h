/*
 * Lap_Timer.h
 *
 *  Created on: 14.07.2013
 *      Author: kolja
 */

#ifndef LAP_TIMER_H_
#define LAP_TIMER_H_
class LapTimer{
public:
	LapTimer();
	~LapTimer();
	void race_loop();
	void draw_waiting_screen();
	void waiting_on_speed_up();
	void update_screen();
	void calc_best_theoretical_lap_time();
	int update_sector_time(uint8_t sector_id, uint32_t sector_time, unsigned char* filename);
	int add_sector(uint32_t latitude, uint32_t longitude, unsigned char* filename);
	int get_sector_data(uint8_t sector_id, uint32_t* latitude,uint32_t* longitude,uint32_t* sector_time, unsigned char* filename);
	void update_screen(uint8_t level);
	void init_screen();


private:
	uint8_t 	sector_count;
	uint8_t 	current_sector;
	uint8_t		last_dist_to_target; // TODO init?
	uint32_t	sector_end_latitude;
	uint32_t 	sector_end_longitude;
	uint32_t 	best_sector_time_ms;
	uint32_t 	best_theoretical_lap_time_ms;
	uint32_t	sector_start_timestamp_ms;
	uint32_t	lap_start_timestamp_ms;
	uint32_t	starting_standing_timestamp_s;
	int32_t		delay_ms;
	bool	 	delay_calc_active;
	unsigned char filename[20]; // "/NAVI/HOCKENHE.SST"
};

#endif /* LapTimer */
