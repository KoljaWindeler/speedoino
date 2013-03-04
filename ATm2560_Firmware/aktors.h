/*
 * aktors.h
 *
 *  Created on: 16.11.2011
 *      Author: jkw
 */

#ifndef AKTORS_H_
#define AKTORS_H_

#define RGB_OUT_R 7
#define RGB_OUT_G 8
#define RGB_OUT_B 9
#define RGB_IN_W 13

#define ATM328RESETPIN PD4
#define PORT_REP_ADDR 0x20				// I2C Address of MCP23017 Chip
#define PORT_REP_ADDR_GPIO_A 0x12     // Register Address of Port A
#define PORT_REP_ADDR_GPIO_B 0x13     // Register Address of Port B
#define FLASHER_RIGHT 3 // bit in register
#define HIGH_BEAM 4 // bit in register
#define NEUTRAL_GEAR 5 // bit in register
#define FLASHER_LEFT 6 // bit in register
#define OIL 7 // bit in register



typedef struct{
	unsigned char actual;
	unsigned char from;
	unsigned char to;
} led_values;

typedef struct{
	unsigned char r;
	unsigned char g;
	unsigned char b;
} led_simple;

typedef struct{
	led_values r;
	led_values g;
	led_values b;
} led;



class Speedo_aktors{
public:
	Speedo_aktors(void);
	~Speedo_aktors();
	void init();
	void clear_vars();
	bool check_vars();
	void set_rgb_in(int r,int g,int b);
	void set_rgb_in(int r,int g,int b,int save);
	void set_rgb_out(int r,int g,int b);
	void set_rgb_out(int r,int g,int b,int save);
	void dimm_rgb_to(int r,int g,int b,int max_dimm_steps);
	void timer_overflow();
	void run_reset_on_ATm328();
	bool dimm_available();
	int  update_outer_leds(bool dimm,bool overwrite);
	int set_bt_pin();
	int ask_bt(char *command);
	void stop_dimmer();
	void set_active_dimmer(bool state);
	int set_expander();
	int set_controll_lights(unsigned char oil,unsigned char flasher_left,unsigned char n_gear,unsigned char flasher_right,unsigned char high_beam);
	int set_rbg_active(int status);
	speedo_stepper* m_stepper;
	speedo_oiler* m_oiler;
	led RGB;
	led_simple dz_flasher,oil_start_color,oil_end_color,water_start_color,water_end_color,kmh_start_color,kmh_end_color,dz_start_color,dz_end_color,static_color;
	short int led_mode;
	int oil_max_value,oil_min_value,water_max_value,water_min_value,kmh_max_value,kmh_min_value,dz_max_value,dz_min_value,bt_pin;

private:
	int dimm_steps,dimm_step,in_out;
	short int dimm_state;
	bool colorfade_active;
	unsigned char control_lights;
	unsigned char led_area_controll;

};
extern Speedo_aktors* pAktors;

#endif /* AKTORS_H_ */
