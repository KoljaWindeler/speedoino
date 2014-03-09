/*
 * speedo.h
 *
 *  Created on: 01.06.2011
 *      Author: kolja
 */

#ifndef SPEEDO_H_
#define SPEEDO_H_

typedef struct {
	int8_t x;
	int8_t y;
	int8_t font;
	bool symbol;
} widget;

//109982 - 4737
//110308 - 4899 :( --struct
//110414 - 4953 :(( -- kmhchars
//109088 - 4467 :] -- die elseif geschichte gelöscht -> nur ein test, keine reale variante
//110432 - 4245 :D --string in flash
//111696 - 3715 :D --string in flash


class speedo_speedo{

#define       	OIL_TEMP 0
#define       	AIR_TEMP 1
#define       	SPEED_VALUE 2
#define       	DZ_VALUE 3
#define       	ADD_INFO 4
#define       	ADD_INFO2 5
#define       	TIME_BAK 6
#define       	GEAR_VALUE 7
#define       	FUEL_VALUE 8
#define		  	ARROW 9
#define		  	WATER_TEMP 10
#define			GPS_VALUE 11

public:
	void loop(unsigned long previousMillis);
	void reset_bak();
	void clear_vars();
	void check_vars();
	void initial_draw_screen();
	int 	  	  m_trip_mode; // welcher mode .. ob zeit, avg oder was auch immer
	int	  		  m_trip_storage; // welcher wert angezeigt wird, 1= day 2=bla 3=blabla etc
	int 		  disp_zeile_bak[12];// backup bestimmter werte um abzuschätzen ob die Zeile geupdated werden sollte
	int           max_speed[9];      // array of the max speed values sollte dem speedo_speedo gehören
	unsigned long avg_timebase[9];
	unsigned long trip_dist[9]; // 0=Total, 1=Non permanent, 2=Day, 3=Tour, 4=Quick, 5=Fuel, 6=Oiler, 7=Saison, 8=Board
	int 		  refresh_cycle;
	bool 		  startup_by_ignition;

	int maximum;
	int minimum;

	widget		kmhchar_widget,oil_widget,water_widget,air_widget,arrow_widget,kmh_widget,dz_widget;
	widget		fuel_widget,gear_widget,addinfo_widget,addinfo2_widget,clock_widget,gps_widget;
	int8_t default_font;
private:
	bool check_no_collision_with_addinfo2(int current_widget_y);
	bool addinfo2_currently_shown;

};
extern speedo_speedo* pSpeedo;

#endif /* SPEEDO_H_ */
