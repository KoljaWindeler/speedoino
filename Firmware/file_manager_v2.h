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
	#define ST_MSG_SIZE_1	2
	#define ST_MSG_SIZE_2	3
	#define ST_GET_TOKEN	4
	#define ST_GET_DATA		5
	#define ST_GET_CHECK	6
	#define ST_PROCESS		7

	#define MESSAGE_START			0x1B        //= ESC = 27 decimal
	#define TOKEN					0x0E

	#define CMD_SIGN_ON				0x01
	#define CMD_LEAVE_FM			0x04
	#define CMD_GO_LEFT				0x05

	#define STATUS_CMD_OK           0x00
	#define STATUS_CMD_FAILED       0xC0
	#define STATUS_CKSUM_ERROR      0xC1
	#define STATUS_CMD_UNKNOWN      0xC9

	#define WAIT_MS_FOR_DATA		2000


//#define CMD_SET_PARAMETER                   0x02
//#define CMD_GET_PARAMETER                   0x03
//#define CMD_SET_DEVICE_PARAMETERS           0x04
//#define CMD_OSCCAL                          0x05
//#define CMD_LOAD_ADDRESS                    0x06
//#define CMD_FIRMWARE_UPGRADE                0x07
public:
	speedo_filemanager_v2();
	~speedo_filemanager_v2();
	void get_filename(char* buffer);
	void run();
	void parse_command();
private:
	bool cd(char dir[20]);
	bool recv_file(char filename[13]);
	bool send_file(char filename[13]);
	bool ls();
	bool mkdir(char dir[20]);
	bool rm(char filename[13]);
	bool rmdir();
	SdFile fm_handle;
	SdFile fm_file;
};
extern speedo_filemanager_v2* pFilemanager_v2;


#endif /* FILE_MANAGER_V2_H_ */
