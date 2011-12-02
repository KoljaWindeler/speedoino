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
#define RGB_IN_R 13
#define RGB_IN_G 10
#define RGB_IN_B 11

typedef struct{
	unsigned char actual;
	unsigned char from;
	unsigned char to;
} led_values;

typedef struct{
	led_values r;
	led_values g;
	led_values b;
} led;

typedef struct{
	led inner;
	led outer;
} led_area;


class Speedo_aktors{
public:
	Speedo_aktors(void);
	~Speedo_aktors();
	void init();
	void set_rgb_in(int r,int g,int b);
	void set_rgb_in(int r,int g,int b,int save);
	void set_rgb_out(int r,int g,int b);
	void set_rgb_out(int r,int g,int b,int save);
	void dimm_rgb_to(int r,int g,int b,int max_dimm_steps, int set_in_out);
	void timer_overflow();
	bool dimm_available();
	speedo_stepper* m_stepper;
	led_area RGB;
	led dz_flasher,out_base_color; // doof aber nötig

private:
	int dimm_steps,dimm_step,in_out;

};
extern Speedo_aktors* pAktors;

#endif /* AKTORS_H_ */
