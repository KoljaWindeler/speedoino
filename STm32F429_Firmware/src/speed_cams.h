/*
 * SpeedCams.h
 *
 *  Created on: 01.06.2011
 *      Author: kolja
 */

#ifndef SPEEDCAMS_H_
#define SPEEDCAMS_H_

#define SPEEDCAM_STATE_INIT 99
#define SPEEDCAM_STATE_START 0
#define SPEEDCAM_STATE_READFILE_OPEN 1
#define SPEEDCAM_STATE_ERROR_OPEN_READFILE -1
#define SPEEDCAM_STATE_ERROR_OPEN_WRITEFILE -2
#define SPEEDCAM_STATE_ERROR_WRITEFILE_SEEK -3
#define	SPEEDCAM_STATE_ERROR_WRITE_WRITEFILE -4

#define WARNING_DIST 500
#define USE_SORTED_FILE

typedef struct {
	uint32_t dest_file_seek;
	uint16_t POIs_parsed;
	int8_t state;
	bool dest_file_open;
	bool in_longitude_section;
	bool running;
} speed_cam_state;


class speedcams{

public:
	speedcams();
	bool calc();
	void set_gps_outdated();
	bool get_active();
	uint16_t get_dist_to_next_point();
	void set_active(bool outer_active);
	void override_start();
	void interface();

private:
	bool gps_outdated;
	bool POI_near;
	bool active;
	uint16_t POI_near_dist;
	uint8_t POI_near_id;
	uint16_t bestOfThree_retrigger_distance;

	speed_cam_state b2s_status;
	simple_coordinate top_three[3];
	simple_coordinate db_last_calc;
	simple_coordinate bestOfThree_last_calc;

	FIL source_file;
	FIL dest_file;

	int8_t parse_complete_db();
	int8_t parse_small_db();

};
extern speedcams SpeedCams;

#endif /* SPEEDCAMS_H_ */
