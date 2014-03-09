/*
 * tetris.h
 *
 *  Created on: 03.12.2011
 *      Author: jkw
 */

#ifndef TETRIS_H_
#define TETRIS_H_

class tetris{

#define LINES 16
#define COLS 12
#define LEFT_OFFSET

public:
	tetris();
	~tetris();
	void run();
private:
	bool loop();
	void init();
	void updateField();
	void initDrawField();
	void check_stack();
	void new_element();
	void show_grid_on_serial();
	void copy_element_to_buffer(int this_element);
	int  get_dir();
	signed int area[16];  // <-> 12  und 16 in der höhe, jede zeile ist ein int ..
	bool active_element[3][3];
	signed int active_x,active_y;// obere linke ecke des aktiven 4x4 elements
	unsigned long last_update;
	short next_element,this_element;
	bool you_loose;
	int lines;
	int time_between_steps;

};

#endif /* TETRIS_H_ */
