/*
 * akting.h
 *
 *  Created on: 16.11.2011
 *      Author: jkw
 */

#ifndef akting_H_
#define akting_H_

#include "oiler.h"
#include "stepper.h"

#define RGB_ACTION_TYPE_STATIC 0
#define RGB_ACTION_TYPE_FOLLOW 1
#define RGB_ACTION_TYPE_STACK 2
#define RGB_ACTION_TYPE_RSTACK 3

#define RGB_OUT_R_PIN PH4		// 7
#define RGB_OUT_R_PORT PORTH
#define RGB_OUT_R_DIR DDRH

#define RGB_OUT_G_PIN PH5		// 8
#define RGB_OUT_G_PORT PORTH
#define RGB_OUT_G_DIR DDRH

#define RGB_OUT_B_PIN PH6		// 9
#define RGB_OUT_B_PORT PORTH
#define RGB_OUT_B_DIR DDRH

#define RGB_IN_W_PIN PB7		// 13
#define RGB_IN_W_PORT PORTB
#define RGB_IN_W_DIR DDRB

#define RESET_KICK_TO_RESET 0x01
#define RESET_COMPLETE 0x02
#define RESET_PREPARE 0x03

#define ATM328RESETPIN PD4
#define PORT_REP_ADDR 0x20				// I2C Address of MCP23017 Chip
#define PORT_REP_ADDR_GPIO_A 0x12     // Register Address of Port A
#define PORT_REP_ADDR_GPIO_B 0x13     // Register Address of Port B
#define FLASHER_RIGHT 3 // bit in register
#define HIGH_BEAM 4 // bit in register
#define NEUTRAL_GEAR 5 // bit in register
#define FLASHER_LEFT 6 // bit in register
#define OIL 7 // bit in register

#define FLASH_COLOR_REACHED 0	// dimm states
#define DIMM_TO_FLASH_COLOR 1
#define STATIC_COLOR_REACHED 2
#define DIMM_TO_STATIC_COLOR 3
#define DIMM_TO_DARK 4
#define MIN_REACHED 5
#define MAX_REACHED 6



typedef struct{
	uint8_t actual;
	uint8_t from;
	uint8_t to;
} led_values;

typedef struct{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} led_simple;

typedef struct{
	led_values r;
	led_values g;
	led_values b;
} led;


class akting{
public:
	akting(void);
	~akting();
	void init();
	bool check_vars();
	void set_rgb_in(int r,int g,int b);
	void set_rgb_in(int r,int g,int b,int save);
	void set_rgb_out(int r,int g,int b);
	void set_rgb_out(int r,int g,int b,int save);
	void dimm_rgb_to(int r,int g,int b,int max_dimm_steps);
	void timer_overflow();
	void run_reset_on_ATm328(char mode);
	bool dimm_available();
	int  update_outer_leds(bool dimm,bool overwrite);
	int set_bt_pin();
	void check_flag();
	bool check_mac_key(uint8_t* result,bool* comm_error);
	int ask_bt(char *command);
	int ask_bt(char *buffer, bool answere_needed, int8_t max_length, uint8_t* char_rec);
	void stop_dimmer();
	void set_active_dimmer(bool state);
	int set_expander();
	int set_controll_lights(uint8_t oil,uint8_t flasher_left,uint8_t n_gear,uint8_t flasher_right,uint8_t high_beam, bool now);
	int set_rbg_active(int status, bool now);
	void rgb_action(int needle_pos);
	stepper mStepper;
	oiler mOiler;
	led RGB;
	led_simple dz_flasher,oil_start_color,oil_end_color,water_start_color,water_end_color,kmh_start_color,kmh_end_color,dz_start_color,dz_end_color,static_color;
	int8_t led_mode,pointer_highlight_mode;
	int16_t oil_max_value,oil_min_value,water_max_value,water_min_value,kmh_max_value,kmh_min_value,dz_max_value,dz_min_value,bt_pin;
	bool attention_required;

private:
	int current_sensor_value;
	int dimm_steps,dimm_step;
	short int dimm_state;
	bool colorfade_active;
	bool expander_outdated;
	uint8_t control_lights;
	uint8_t led_area_controll;

};
extern akting Aktors;

#endif /* akting_H_ */
