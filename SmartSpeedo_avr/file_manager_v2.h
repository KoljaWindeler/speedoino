/*
 * file_manager_v2.h
 *
 *  Created on: 14.01.2012
 *      Author: jkw
 */

#ifndef FILE_MANAGER_V2_H_
#define FILE_MANAGER_V2_H_

class speedo_filemanager_v2{

#define ST_START		0
#define ST_GET_SEQ_NUM	1
#define ST_MSG_SIZE		2
#define ST_MSG_SIZE_2	3
#define ST_GET_TOKEN	4
#define ST_GET_DATA		5
#define ST_GET_CHECK	6
#define ST_PROCESS		7

#define MESSAGE_START			0x1B        //= ESC = 27 decimal
#define TOKEN					0x0E

#define STATUS_CMD_OK          	0x00
#define STATUS_EOF		       	0x10
#define STATUS_CMD_FAILED      	0xC0
#define STATUS_CKSUM_ERROR     	0xC1
#define STATUS_CMD_UNKNOWN     	0xC9

#define CMD_SIGN_ON				0x01
#define CMD_LEAVE_FM			0x04
#define CMD_LOAD_ADDRESS		0x06
#define CMD_LEAVE_PROGMODE_ISP	0x11
#define CMD_CHIP_ERASE_ISP		0x12
#define CMD_PROGRAM_FLASH_ISP	0x13
#define CMD_SPI_MULTI			0x1D
#define CMD_GO_LEFT				0x25
#define CMD_GO_RIGHT			0x26
#define CMD_GO_UP				0x27
#define CMD_GO_DOWN				0x28
#define CMD_DIR					0x31
#define CMD_GET_FILE			0x32
#define CMD_PUT_FILE			0x33
#define CMD_DEL_FILE			0x34
#define CMD_SHOW_GFX			0x35
#define CMD_FILE_RECEIVE		0x39
#define CMD_RESET_SMALL_AVR		0x40
#define CMD_SIGN_ON_FIRMWARE	0x41
#define CMD_SET_STARTUP 		0x42
#define CMD_POST_OIL_R			0x43
#define CMD_POST_WATER_R		0x44
#define CMD_POST_WATER_TEMP		0x45
#define CMD_POST_AIR_TEMP		0x46
#define CMD_POST_SPEED			0x47
#define CMD_POST_RPM			0x48
#define CMD_POST_SPEED_PSEUDO	0x49
#define CMD_POST_LIGHTS			0x50

#define CMD_PULSE_OILER 		0x51

#define FAILURE_FILE_OPEN 0x01
#define FAILURE_FILE_SEEK 0x02
#define FAILURE_FILE_READ 0x03


#define WAIT_MS_FOR_DATA		2000


#define CMD_SET_PARAMETER                   0x02
#define CMD_GET_PARAMETER                   0x03
#define CMD_SET_DEVICE_PARAMETERS           0x04
	//#define CMD_OSCCAL                          0x05
	//#define CMD_LOAD_ADDRESS                    0x06
	//#define CMD_FIRMWARE_UPGRADE                0x07
public:
	speedo_filemanager_v2();
	~speedo_filemanager_v2();
	//void get_filename(char* buffer);
	void run();
	void parse_command();
	void check_input();
	int send_answere(unsigned char *msgBuffer,unsigned int msgLength);
	int send_answere(unsigned char *msgBuffer,unsigned int msgLength,unsigned char *seqNum, unsigned char *msgParseState);
private:

	unsigned char	msgParseState;
	unsigned char	seqNum;
};
extern speedo_filemanager_v2* pFilemanager_v2;


#endif /* FILE_MANAGER_V2_H_ */
