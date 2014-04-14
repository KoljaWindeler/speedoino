/*
 * aktors.h
 *
 *  Created on: 16.11.2011
 *      Author: jkw
 */

#ifndef AKTORS_H_
#define AKTORS_H_

#define RESET_KICK_TO_RESET 0x01
#define RESET_COMPLETE 0x02
#define RESET_PREPARE 0x03

#define ATM328RESETPIN PD4
#define FLASHER_RIGHT 3 // bit in register
#define HIGH_BEAM 4 // bit in register
#define NEUTRAL_GEAR 5 // bit in register
#define FLASHER_LEFT 6 // bit in register
#define OIL 7 // bit in register



class Speedo_aktors{
public:
	Speedo_aktors(void);
	~Speedo_aktors();
	void init();
	void run_reset_on_ATm328(char mode);
	int set_bt_pin();
	int set_bt_pin(bool reset);
	bool check_mac_key(uint8_t* result,bool* comm_error);
	bool check_bt_connection();
	int ask_bt(char *command);
	int ask_bt(char *buffer, bool answere_needed, int8_t max_length, uint8_t* char_rec);
	speedo_oiler* m_oiler;

private:

};
extern Speedo_aktors* pAktors;

#endif /* AKTORS_H_ */
