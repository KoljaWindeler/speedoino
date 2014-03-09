/*
 * tetris.cpp
 *
 *  Created on: 03.12.2011
 *      Author: jkw
 */
#include "global.h"
#define MIN_SIDE_PUSH_TIME 160
#define MIN_TURN_PUSH_TIME 250
#define MIN_DOWN_PUSH_TIME 60

tetris::tetris(){	};
tetris::~tetris(){	};

// init routine to reset variables
void tetris::init(){
	// clear area
	for(unsigned int i=0;i<sizeof(area)/sizeof(area[0]);i++){
		area[i]=0b0000000000000000;
	};
	// reset vars
	lines=0;
	last_update=millis();
	time_between_steps=999;
	you_loose=false;
	// load last element
	new_element();
	// load this element
	new_element();
	pSpeedo->reset_bak();
};

// this function creats the stones
void tetris::copy_element_to_buffer(int this_element){
	switch(this_element){
	case 0:
		active_element[0][0]=false; //  x
		active_element[0][1]=true;  // xx
		active_element[0][2]=false; // x
		active_element[1][0]=true;
		active_element[1][1]=true;
		active_element[1][2]=false;
		active_element[2][0]=true;
		active_element[2][1]=false;
		active_element[2][2]=false;
		break;
	case 1:
		active_element[0][0]=true;  // x
		active_element[0][1]=false; // xx
		active_element[0][2]=false; //  x
		active_element[1][0]=true;
		active_element[1][1]=true;
		active_element[1][2]=false;
		active_element[2][0]=false;
		active_element[2][1]=true;
		active_element[2][2]=false;
		break;
	case 2:
		active_element[0][0]=true;  // xx
		active_element[0][1]=true;  // xx
		active_element[0][2]=false; //
		active_element[1][0]=true;
		active_element[1][1]=true;
		active_element[1][2]=false;
		active_element[2][0]=false;
		active_element[2][1]=false;
		active_element[2][2]=false;
		break;
	case 3:
		active_element[0][0]=true;   // x
		active_element[0][1]=false;  // x
		active_element[0][2]=false;  // xx
		active_element[1][0]=true;
		active_element[1][1]=false;
		active_element[1][2]=false;
		active_element[2][0]=true;
		active_element[2][1]=true;
		active_element[2][2]=false;
		break;
	case 4:
		active_element[0][0]=true;   //  x
		active_element[0][1]=true;   //  x
		active_element[0][2]=false;  // xx
		active_element[1][0]=true;
		active_element[1][1]=false;
		active_element[1][2]=false;
		active_element[2][0]=true;
		active_element[2][1]=false;
		active_element[2][2]=false;
		break;
	case 5:
		active_element[0][0]=true;   // x
		active_element[0][1]=false;  // x
		active_element[0][2]=false;  // x
		active_element[1][0]=true;
		active_element[1][1]=false;
		active_element[1][2]=false;
		active_element[2][0]=true;
		active_element[2][1]=false;
		active_element[2][2]=false;
		break;
	case 6:
		active_element[0][0]=true;   // x
		active_element[0][1]=false;  // xx
		active_element[0][2]=false;  // x
		active_element[1][0]=true;
		active_element[1][1]=true;
		active_element[1][2]=false;
		active_element[2][0]=true;
		active_element[2][1]=false;
		active_element[2][2]=false;
		break;
	}
}

void tetris::new_element(){
	// start conditions
	active_x=5;
	active_y=-1;

	// generate numbers
	int element=millis()%7;
	this_element=next_element;
	next_element=element;

	// copy to display
	copy_element_to_buffer(next_element);

	pOLED->filled_rect(100,31,12,12,0);
	for(int x=0;x<3;x++){
		for(int y=0;y<3;y++){
			if(active_element[x][y]){
				pOLED->filled_rect(x*4+100,y*4+31,4,4,0x0f);
			} else {
				pOLED->filled_rect(x*4+100,y*4+31,4,4,0x00);
			}
		}
	}

	// copy to play
	copy_element_to_buffer(this_element);
}


bool tetris::loop(){
	char c=0x00;
	int state=0;
	while(Serial.available()>0){
		_delay_ms(5);
		if(state==0){
			//pOLED->string(0,"0",0,0);

			if(Serial.read()==MESSAGE_START){	state=1;	} else {	state=0;	}
		} else if(state==1){
			//pOLED->string(0,"1",0,0);

			if(Serial.read()==0x01){	state=2;	} else {	state=0;	}
		} else if(state==2){
			//pOLED->string(0,"2",0,0);

			if(Serial.read()==0x00){	state=3;	} else {	state=0;	}
		} else if(state==3){
			//pOLED->string(0,"3",0,0);

			if(Serial.read()==0x01){	state=4;	} else {	state=0;	}
		} else if(state==4){
			//pOLED->string(0,"4",0,0);

			if(Serial.read()==TOKEN){	state=5;	} else {	state=0;	}
		} else if(state==5){
			//pOLED->string(0,"5",0,0);
			char d=Serial.read();

			if(d==CMD_GO_LEFT){
				//pOLED->string(0,"6",0,0);
				c=1;
				state=6;
			} else if(d==CMD_GO_RIGHT){
				//pOLED->string(0,"6",0,0);
				c=2;
				state=6;
			} else if(d==CMD_GO_UP){
				//pOLED->string(0,"6",0,0);
				c=3;
				state=6;
			} else if(d==CMD_GO_DOWN){
				//pOLED->string(0,"6",0,0);
				c=4;
				state=6;
			} else {	state=0; c=0;	}
		} else if(state==6){
			//pOLED->string(0,"7",0,0);

			state=0;
			Serial.flush();
			break;
		}
		// left
		// right
		// up
		// down
	};

	// check if you are stil in the race
	if(you_loose){
		// Nope, you loose show the box once
		if(pSpeedo->disp_zeile_bak[0]!=123){
			// show a animation depending on line count
			int ani=1; // simpsons
			if(lines>50){
				ani=4; // JTM
			} else if(lines>30){
				ani=5;
			}
			// 1sec geben damit der user realisiert
			// dann sicher sein das er keine taste
			// mehr drückt
			_delay_ms(1000);
			while(Serial.available()>0 || 	!(PINJ & (1<<menu_button_links)) || !(PINJ & (1<<menu_button_rechts)) || !(PINJ & (1<<menu_button_oben)) ||	!(PINJ & (1<<menu_button_unten)) ){
					Serial.flush();
				_delay_ms(50);
			}
			pOLED->animation(ani);
			initDrawField(); // draw the field again, to show the line and level counter

			// make sure to draw this box only once
			pSpeedo->disp_zeile_bak[0]=123;
			// draw box
			pOLED->highlight_bar(8,8,104,48);
			pOLED->string_P(pSpeedo->default_font,PSTR("You loose"),5,2,15,0,0);
			pOLED->string_P(pSpeedo->default_font,PSTR("L Quit"),5,4,15,0,0);
			char temp[2];
			sprintf(temp,"%c",126);
			pOLED->string(pSpeedo->default_font,temp,5,4,15,0,0);
			pOLED->string_P(pSpeedo->default_font,PSTR("R Retry"),5,5,15,0,0);
			sprintf(temp,"%c",127);
			pOLED->string(pSpeedo->default_font,temp,5,5,15,0,0);
			// way at least one second to prevent unnoticed button push
			_delay_ms(1000);
			// if you loose and the box has been drawn, way on key down
		} else {
			if(c==1 || !(PINJ & (1<<menu_button_links))){
				_delay_ms(MIN_SIDE_PUSH_TIME);
				return false;
			} else if (c==2 || !(PINJ & (1<<menu_button_rechts))){
				_delay_ms(MIN_SIDE_PUSH_TIME);
				pOLED->clear_screen();
				// ja das ist ungeschickt, init leert uns die line variable, drawfield malt das hin, das problem ist nur
				// das wir mit drawfield die in init gezeichneten "next" übermalen .. also einfach 2x init .. is ja wurst
				init();
				initDrawField();
				init();
				updateField();
			};
		}
		// nope you haven't lost by now .. go on
	} else {
		////////////////// if there is a button pushed, //////////////////
		if(!(PINJ & (1<<menu_button_links))){
			c=1;
			_delay_ms(MIN_SIDE_PUSH_TIME);
		} else if(!(PINJ & (1<<menu_button_oben))){
			c=3;
			_delay_ms(MIN_TURN_PUSH_TIME);
		} else if(!(PINJ & (1<<menu_button_unten))){
			c=4;
			_delay_ms(MIN_DOWN_PUSH_TIME);
		} else if(!(PINJ & (1<<menu_button_rechts))){
			c=2;
			_delay_ms(MIN_SIDE_PUSH_TIME);
		}

		// now lets see if any action is required
		////////////////// rotate element //////////////////
		if(c==3){
			// element drehen
			bool copy[3][3];
			copy[0][0]=active_element[0][2];	//	00	10	20  		02	01	00
			copy[1][0]=active_element[0][1];	//  01	11	21   ==>	12	11	10
			copy[2][0]=active_element[0][0];	//	02	12	22			22	21	20
			copy[0][1]=active_element[1][2];
			copy[1][1]=active_element[1][1];
			copy[2][1]=active_element[1][0];
			copy[0][2]=active_element[2][2];
			copy[1][2]=active_element[2][1];
			copy[2][2]=active_element[2][0];

			// check if a rotation would result in a collision
			bool rotate_possible=true;
			bool check_running=true;

			for(int y=0;y<3 && check_running;y++){
				for(int x=0;x<3 && check_running;x++){
					// check if in the rotated figure the px is in use
					if(copy[x][y]){
						// if so, check if it is already in use by the area
						if(area[active_y+y] & 1<<(COLS-active_x-x)){
							// if so, rotation is impossible
							rotate_possible=false;
							check_running=false;
						};
					};
				};
			};

			// if rotation is possible, move the content from copy -> active_element
			if(rotate_possible){
				for(int x=0;x<3;x++){
					for(int y=0;y<3;y++){
						active_element[x][y]=copy[x][y];
					}
				};
				// element drehen
				updateField();
			}

			////////////////// move element down //////////////////
		} else if(c==4){
			if(pSpeedo->disp_zeile_bak[1]!=111){
				active_y++;
			};
			updateField();
			check_stack();
			last_update=millis();

			////////////////// move element left //////////////////
		} else if(c==1){
			// find out the leftmost positions
			short most_left[3];
			for(int y=0;y<3;y++){
				most_left[y]=-1;

				for(int x=0;x<3;x++){
					if(active_element[x][y]){
						most_left[y]=x;
						break;
					}
				}
			}

			// check collisions
			if(!((most_left[0]!=-1 && (area[active_y+0] & (1<<(COLS-active_x+1-most_left[0])))) ||
					(most_left[1]!=-1 && (area[active_y+1] & (1<<(COLS-active_x+1-most_left[1])))) ||
					(most_left[2]!=-1 && (area[active_y+2] & (1<<(COLS-active_x+1-most_left[2])))))){
				active_x--;
			};

			if(active_x<0){
				active_x=0;
				if(!active_element[0][0] && !active_element[0][1] && !active_element[0][2]){
					// 1. reihe inhalt der 0. reihe
					active_element[0][0]=active_element[1][0];
					active_element[0][1]=active_element[1][1];
					active_element[0][2]=active_element[1][2];
					// 2.reihe mit dem inhalt der 1. reihe
					active_element[1][0]=active_element[2][0];
					active_element[1][1]=active_element[2][1];
					active_element[1][2]=active_element[2][2];
					// 0. Reihe nullen
					active_element[2][0]=false;
					active_element[2][1]=false;
					active_element[2][2]=false;
				}
			};
			updateField();

			////////////////// move element right //////////////////
		} else if(c==2){
			// find out the leftmost positions
			short most_right[3];
			for(int y=0;y<3;y++){
				most_right[y]=-1;

				for(int x=2;x>=0;x--){
					if(active_element[x][y]){
						most_right[y]=x;
						break;
					}
				}
			}

			// check collisions
			if(!((most_right[0]!=-1 && (area[active_y+0] & (1<<(COLS-active_x-1-most_right[0])))) ||
					(most_right[1]!=-1 && (area[active_y+1] & (1<<(COLS-active_x-1-most_right[1])))) ||
					(most_right[2]!=-1 && (area[active_y+2] & (1<<(COLS-active_x-1-most_right[2])))))){
				active_x++;
			};

			if(active_x>9){
				active_x=9;
				// wenn wir eigentlich ganz nach rechts wollten, dann versuch dochmal
				// den inhalt weiter nach rechts zu schieben
				if(!active_element[2][0] && !active_element[2][1] && !active_element[2][2]){
					// 2.reihe mit dem inhalt der 1. reihe
					active_element[2][0]=active_element[1][0];
					active_element[2][1]=active_element[1][1];
					active_element[2][2]=active_element[1][2];
					// 1. reihe inhalt der 0. reihe
					active_element[1][0]=active_element[0][0];
					active_element[1][1]=active_element[0][1];
					active_element[1][2]=active_element[0][2];
					// 0. Reihe nullen
					active_element[0][0]=false;
					active_element[0][1]=false;
					active_element[0][2]=false;
				}
			}

			updateField();
		}
		////////////////// auto move down //////////////////
		// einen tiefer setzen nach ablauf von zeit
		if(last_update+time_between_steps<millis()){
			check_stack(); // erst checken, dann verschieben
			active_y++;
			updateField();
			last_update=millis();
			// 100ms nach dem automatisch runtersetzen checken obs kollisionen gibt
		};
	};

	return true;
};

void tetris::run(){
	initDrawField();
	init();
	updateField();

	while(loop()){
		pSensors->m_reset->toggle();
	}
}

void tetris::initDrawField(){
	pOLED->clear_screen();
	// Feld ist (12*4)x(16*4) == 48x64 liegt zwischen 12..54 | 60..64
	// .. 0
	// .x 1
	// xx 2
	// x. 3
	// .. 4
	// xx 5
	// .x 6
	// .x 7
	// .. 8
	// x. 9
	// xx 10
	// x. 11
	// .. 12
	// xx 13
	// x. 14
	// x. 15
	pOLED->filled_rect(0,4,8,64,0x06);
	pOLED->filled_rect(0,1*4,4,4,0x00);
	pOLED->filled_rect(0,4*4,4,4,0x00);
	pOLED->filled_rect(0,6*4,4,12,0x00);
	pOLED->filled_rect(0,12*4,4,4,0x00);

	pOLED->filled_rect(4,3*4,4,8,0x00);
	pOLED->filled_rect(4,8*4,4,8,0x00);
	pOLED->filled_rect(4,11*4,4,8,0x00);
	pOLED->filled_rect(4,14*4,4,8,0x00);




	for(int y=0;y<64;y++){
		pOLED->set2pixels(8,y,0x00,0x0f);
		pOLED->set2pixels(10,y,0x00,0x0f);

		pOLED->set2pixels(60,y,0x0f,0x00);
		pOLED->set2pixels(62,y,0x0f,0x00);
	}


	pOLED->string_P(pSpeedo->default_font,PSTR("Speedoino"),11,0,0,15,0);
	pOLED->string_P(pSpeedo->default_font,PSTR("goes "),13,1,0,15,0);
	pOLED->string_P(pSpeedo->default_font,PSTR("Tetris"),12,2,0,15,0);
	pOLED->string_P(pSpeedo->default_font,PSTR("Next"),11,4,0,15,0);
	pOLED->string_P(pSpeedo->default_font,PSTR("Level: "),11,6,0,15,0);
	pOLED->string_P(pSpeedo->default_font,PSTR("Lines: "),11,7,0,15,0);
	char temp[3];
	sprintf(temp,"%2i",int(floor(lines/10)));
	pOLED->string(pSpeedo->default_font,temp,19,6,0,15,0);
	sprintf(temp,"%2i",lines);
	pOLED->string(pSpeedo->default_font,temp,19,7,0,15,0);
}

void tetris::updateField(){
	for(int x=0;x<5;x++){		// 0..4
		for(int y=0;y<5;y++){	// 0..4
			unsigned char color=0x00;

			int internal_x=x+active_x-1;
			int internal_y=y+active_y-1;
			if(internal_x>=0 && internal_y>=0 && internal_x<12){
				if(area[internal_y] & (1<<(COLS-internal_x))){
					color=0x06;
				}
				pOLED->filled_rect(4*(internal_x)+12,4*(internal_y),4,4,color);
			};
		}
	}

	for(int x=0;x<3;x++){
		for(int y=0;y<3;y++){
			if(active_element[x][y]){
				pOLED->filled_rect(4*(active_x+x)+12,4*(active_y+y),4,4,15);
			}
		}
	}
};

void tetris::check_stack(){
	bool stacked=false;
	bool stop_me=false;

	for(int y=0;y<3 && !stop_me;y++){
		for(int x=0;x<3 && !stop_me;x++){
			if(active_element[x][y] && (area[active_y+y+1] & (1 << (COLS-active_x-x)))){
				stacked=true;
			}
			if(active_element[x][y] && (active_y+y==(LINES-1))){
				stacked=true;
			}
			if(stacked){
				//show_grid_on_serial();
				/////////////// copy element to grid ///////////////////
				for(int copy_x=0;copy_x<3;copy_x++){
					for(int copy_y=0;copy_y<3;copy_y++){
						if(active_element[copy_x][copy_y]){
							// area[13]=area[15-2+0]
							// area[14]=area[15-2+1]
							// area[15]=area[15-2+2]
							area[active_y+copy_y] |= (1<<(COLS-active_x-copy_x));
						};
						active_element[copy_x][copy_y]=false;
					};
				};


				// check if any line is completed
				for(int line=0;line<LINES;line++){
					if(area[line]==8190){
						// blinken lassen
						pOLED->filled_rect(12,line*4,12*4,4,0);
						_delay_ms(125);
						pOLED->filled_rect(12,line*4,12*4,4,6);
						_delay_ms(125);
						pOLED->filled_rect(12,line*4,12*4,4,0);
						_delay_ms(125);
						pOLED->filled_rect(12,line*4,12*4,4,6);
						// felder verschieben
						for(int upper_line=line;upper_line>0;upper_line--){
							area[upper_line]=area[upper_line-1];
							// alle linien drüber neu zeichnen
							for(int copy_x=0;copy_x<COLS;copy_x++){
								unsigned char color=0x00;
								if(area[upper_line] & (1<<(COLS-copy_x))){
									color=0x06;
								}
								pOLED->filled_rect(12+copy_x*4,upper_line*4,4,4,color);
							}
						}
						// letzte feld löschen
						area[0]=0b0000000000000000;
						pOLED->filled_rect(12,0,48,4,0);

						// lines hochzählen
						lines++;
						// level schwerer machen :D
						if(lines%10==0){
							time_between_steps=4*time_between_steps/5;
							// 1000
							// 800
							// 640
							// 512
							// 409
							// 327
							// 261
							// 208
							// 166
							// 132
							// 105
							// 084
							// 067
							// 053
						}
						char temp[3];
						sprintf(temp,"%2i",int(floor(lines/10)));
						pOLED->string(pSpeedo->default_font,temp,19,6,0,15,0);
						sprintf(temp,"%2i",lines);
						pOLED->string(pSpeedo->default_font,temp,19,7,0,15,0);
					}
				}

				if(area[0]!=0){
					you_loose=true;
				}

				updateField();

				//show_grid_on_serial();
				new_element();
				stop_me=true;
			};
		}
	}
};

void tetris::show_grid_on_serial(){
	/////////////// SHOW GRID ///////////////////
	pDebug->sprintlnp(PSTR("\n\n"));
	pDebug->sprintlnp(PSTR("===== GRID ====="));
	for(int a_y=0;a_y<LINES;a_y++){
		for(int a_x=0;a_x<COLS;a_x++){
			if(area[a_y] & (1<<(COLS-a_x)))
				Serial.print("x");
			else
				Serial.print(".");
		}
		Serial.println("");
	}
	pDebug->sprintlnp(PSTR("===== GRID ====="));
	pDebug->sprintlnp(PSTR("\n\n"));
	/////////////// SHOW GRID ///////////////////
};


