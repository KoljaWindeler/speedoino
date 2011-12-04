/*
 * tetris.cpp
 *
 *  Created on: 03.12.2011
 *      Author: jkw
 */
#include "global.h"

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
		active_element[0][2]=false;
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
		active_element[0][0]=true;   // xx
		active_element[0][1]=true;   // x
		active_element[0][2]=false;  // x
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

	pOLED->filled_rect(100,30,12,12,0);
	for(int x=0;x<3;x++){
		for(int y=0;y<3;y++){
			if(active_element[x][y]){
				pOLED->filled_rect(x*4+100,y*4+30,4,4,0x0f);
			} else {
				pOLED->filled_rect(x*4+100,y*4+30,4,4,0x00);
			}
		}
	}

	// copy to play
	copy_element_to_buffer(this_element);
}


bool tetris::loop(){
	char c=0x00;
	if(Serial.available()>0){
		c=Serial.read();
		Serial.flush();
	};

	// check if you are stil in the race
	if(you_loose){
		if(pSpeedo->disp_zeile_bak[0]!=123){
			pSpeedo->disp_zeile_bak[0]=123;
			pOLED->highlight_bar(8,8,104,48);
			pOLED->string_P(pSpeedo->default_font,PSTR("You loose"),5,2,15,0,0);
			pOLED->string_P(pSpeedo->default_font,PSTR("L Quit"),5,4,15,0,0);
			char temp[2];
			sprintf(temp,"%c",126);
			pOLED->string(pSpeedo->default_font,temp,5,4,15,0,0);
			pOLED->string_P(pSpeedo->default_font,PSTR("R Retry"),5,5,15,0,0);
			sprintf(temp,"%c",127);
			pOLED->string(pSpeedo->default_font,temp,5,5,15,0,0);
		} else {
			if(c=='a'|| c==68){
				return false;
			} else if (c=='d' || c==67){
				pOLED->clear_screen();
				initDrawField();
				init();
				updateField();
			};
		}
	} else {

		if(digitalRead(menu_button_oben)==LOW || c=='w' || c==65){
			orientation_up=!orientation_up;
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

			for(int x=0;x<3;x++){
				for(int y=0;y<3;y++){
					active_element[x][y]=copy[x][y];
				}
			};
			// element drehen
			updateField();
			//}else if(digitalRead(menu_button_unten)==HIGH || c=='s'){
		} else if(digitalRead(menu_button_unten)==LOW || c=='s' || c==66){
				if(pSpeedo->disp_zeile_bak[1]!=111){
					active_y++;
			};
			updateField();
			check_stack();
			last_update=millis();
			//}else if(digitalRead(menu_button_links)==HIGH || c=='a'){
		} else if(digitalRead(menu_button_links)==LOW || c=='a' || c==68){
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
			//}else if(digitalRead(menu_button_rechts)==HIGH || c=='d'){
		} else if(digitalRead(menu_button_rechts)==LOW || c=='d' || c==67){
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
		// einen tiefer setzen nach ablauf von zeit
		if(last_update+time_between_steps<millis()){
			active_y++;
			updateField();
			last_update=millis();
		// 100ms nach dem automatisch runtersetzen checken obs kollisionen gibt
		} else if(last_update+100<millis()){
			check_stack();
		}
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
	// Feld ist (14*3)x(21*3) == 42x63 liegt zwischen 20..62 | 0..62

	pOLED->filled_rect(8,0,4,64,15); // links
	pOLED->filled_rect(60,0,4,64,15); // rechts

	pOLED->string_P(pSpeedo->default_font,PSTR("Speedoino"),12,0,0,15,0);
	pOLED->string_P(pSpeedo->default_font,PSTR("goes "),12,1,0,15,0);
	pOLED->string_P(pSpeedo->default_font,PSTR("Tetris"),12,2,0,15,0);
	pOLED->string_P(pSpeedo->default_font,PSTR("Next"),12,4,0,15,0);
	pOLED->string_P(pSpeedo->default_font,PSTR("Lines: 0"),12,7,0,15,0);
	char temp[3];
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
					color=0x09;
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
						delay(125);
						pOLED->filled_rect(12,line*4,12*4,4,6);
						delay(125);
						pOLED->filled_rect(12,line*4,12*4,4,0);
						delay(125);
						pOLED->filled_rect(12,line*4,12*4,4,6);
						// felder verschieben
						for(int upper_line=line;upper_line>0;upper_line--){
							area[upper_line]=area[upper_line-1];
							// alle linien drüber neu zeichnen
							for(int copy_x=0;copy_x<COLS;copy_x++){
								unsigned char color=0x00;
								if(area[upper_line] & (1<<COLS-copy_x)){
									color=0x09;
								}
								pOLED->filled_rect(12+copy_x*4,upper_line*4,4,4,color);
							}
						}
						// letzte feld löschen
						area[0]=0b0000000000000000;
						pOLED->filled_rect(20,0,48,4,0);

						// lines hochzählen
						lines++;
						// level schwerer machen :D
						if(lines%10==0){
							time_between_steps=2*time_between_steps/3;
						}
						char temp[3];
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
	Serial.println("\n\n");
	Serial.println("===== GRID =====");
	for(int a_y=0;a_y<LINES;a_y++){
		for(int a_x=0;a_x<COLS;a_x++){
			if(area[a_y] & (1<<(COLS-a_x)))
				Serial.print("x");
			else
				Serial.print(".");
		}
		Serial.println("");
	}
	Serial.println("===== GRID =====");
	Serial.println("\n\n");
	/////////////// SHOW GRID ///////////////////
};
