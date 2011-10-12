/*
 * file_manager.h
 *
 *  Created on: 01.06.2011
 *      Author: kolja
 */

#ifndef FILE_MANAGER_H_
#define FILE_MANAGER_H_

///////////// filemanager ///////////////////////////////////
class speedo_filemanager{
public:
	speedo_filemanager(void);
	~speedo_filemanager();
	void get_filename(char* buffer);
	void run();
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
extern speedo_filemanager* pFilemanager;
///////////// filemanager ///////////////////////////////////

#endif /* FILE_MANAGER_H_ */
