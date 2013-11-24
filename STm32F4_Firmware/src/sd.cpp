/* Speedoino - This file is part of the firmware.
 * Copyright (C) 2011 Kolja Windeler
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "global.h"
#include "ff.h"
#include "diskio.h"

sd::sd(){
};

sd::~sd(){
};

/*flags: FA_OPEN_ALWAYS | FA_WRITE | FA_READ
	FA_READ				Specifies read access to the object. Data can be read from the file. Combine with FA_WRITE for read-write access.
	FA_WRITE			Specifies write access to the object. Data can be written to the file. Combine with FA_READ for read-write access.

	FA_OPEN_EXISTING	Opens the file. Fails if the file is not existing. (Default)
	FA_OPEN_ALWAYS		Opens the file if it is existing. If not, a new file is created. To append data to the file, use f_lseek() function after file open in this method.
	FA_CREATE_NEW		Creates a new file. The function fails with FR_EXIST if the file is existing.
	FA_CREATE_ALWAYS	Creates a new file. If the file is existing, it is truncated and overwritten.
 */

///*
// * Write CR LF to a file
// */
//void sd::writeCRLF(FIL* f) {
//	uint16_t byte_count;
//	f_write(f,(uint8_t*)"\r\n",2,&byte_count);
//}


int8_t sd::append_string(uint8_t* filename,uint8_t* buffer){
	FIL write_file;
	get_file_handle(filename,&write_file,FA_OPEN_ALWAYS | FA_WRITE | FA_READ);	// open and create if not existing
	f_lseek(&write_file,write_file.fsize);										// append -> jump to end
	int8_t status=writeString(&write_file,buffer);								// write the string
	f_close(&write_file);														// close the file
	return status;
}

/*
 * Write a string to a file
 */
int8_t sd::writeString(FIL* f, char* str) {
	return writeString(f,(uint8_t*)str);
}

int8_t sd::writeString(FIL* f, uint8_t* str) {
	uint16_t n;
	unsigned int byte_count;
	for (n = 0; str[n]; n++); // n=zähler, hier sind keine klammern
	return f_write(f,(uint8_t*)str,n,&byte_count);
}

int8_t sd::remove_file(uint8_t* path){
	f_unlink((const char*)path);
	return 0;
}


/* function to read a specific line within a file
 * [in] line_nr: the number, starting with 0 for the first line
 * [in] filename
 * [in] buffer: will hold the content of the specific line
 * [in] lines_in_file: will provide the information about the number of lines in file. might be helpful
 *
 * [return]:
 *  0: good return
 * -1: Open file failed
 * -2: not enough lines in file
 */
int16_t sd::get_line_n(int16_t line_nr,uint8_t* filename,uint8_t* buffer,uint16_t max_length){
	uint16_t temp;
	return get_line_n(line_nr,filename,buffer,max_length,&temp);
}

int16_t sd::get_line_n(int16_t line_nr,uint8_t* filename,uint8_t* buffer,uint16_t max_length, uint16_t* lines_in_file){
	FIL read_file;

	if(get_file_handle(filename,&read_file,FA_READ)<0){
		sd_failed=true;
		return -1;
	}

	bool found=false;
	bool eof=false;
	int16_t cur_line=0;
	int16_t buf_pointer=0;
	uint8_t char_buf[1]={0x00};
	////////// suche den anfang der richtigen zeile
	while(!found && !eof){									// solange suchen bis wir entweder den Datensatz oder das ende haben
		UINT n;
		f_read(&read_file,char_buf, 1,&n);					// jeweils nur 1 uint8_t lesen
		if(n==1){											// true, if not end of file
			if(char_buf[0]==0x0A){							// find end of line
				if(buf_pointer<max_length) buffer[buf_pointer]=0x00;	// if end reach, terminate string
				buf_pointer=0;								// reset buf write pointer
				if(cur_line==line_nr){						// if we have "our" line
					found=true;								// we found the right line
				};
				cur_line++;									// increase number of read lines
			} else {										// its not \n
				buffer[buf_pointer]=char_buf[0];			// so store it in the buffer
				if(buf_pointer<max_length) buf_pointer++;	// and increase the pointer until max
			};
		} else {							// if we were unable to read one further uint8_t
			eof=true;						// we have reached the eof before reading the target line
		};
	}
	f_close(&read_file);
	*lines_in_file=cur_line-1;

	if(eof){
		return -2;
	}
	return 0;
}

void sd::init(){
	// p kanal runterziehen damit er leitend wird
	power_up(3);
};

void sd::power_down(){
	//	pinMode(SD_EN,INPUT); TODO
	//	digitalWrite(SD_EN,HIGH);
}

void sd::power_up(unsigned char tries){
	//	pinMode(SD_EN,OUTPUT); TODO
	//	digitalWrite(SD_EN,LOW);
	sd_failed=false;
	bool allright=true;
	Serial.puts(USART1,("SD try"));
	while(tries>0){ //maximal 3 versuche die sd karte zu öffnen
		Serial.puts(USART1,3-tries);


		int mounted=0;
		allright=true;
		int i=0;
		Serial.puts(USART1,(" mounting "));

		while(!mounted && i<100){
			i++;
			if (f_mount(0, &FatFs) != FR_OK) { // res=0 <- no error, res>0 <- error nr
				allright=false;
				Serial.puts(USART1,("."));
			} else {
				mounted=1;
			}
		}

		if(allright){
			Serial.puts_ln(USART1,(" done!"));
			break;
		} else {
			tries--;
			Serial.puts_ln(USART1,(" failed ,"));
		};
		Sensors.mReset.toggle();// geht das hier schon?
	};
	if(!allright){
		sd_failed=true;
	};

	//	FRESULT ret;
	//	FIL file;
	//	UINT bw, br;
	//	uint8_t buff[128];
	//
	//	if (f_mount(0, &FatFs) != FR_OK) {
	//		Serial.puts_ln(USART1,"could not open filesystem \n\r");
	//	}
	//
	//
	//	/*  hello.txt write test*/
	//	Serial.puts_ln(USART1,"Create a new file (hello.txt)\n\r");
	//	ret = f_open(&file, "HELL.TXT", FA_WRITE | FA_CREATE_ALWAYS);
	//	if (ret) {
	//		Serial.puts_ln(USART1,"Create a new file error\n\r");
	//	} else {
	//		Serial.puts_ln(USART1,"Write a text data. (hello.txt)\n\r");
	//		ret = f_write(&file, "yeah  world Kolja!", 17, &bw);
	//		if (ret) {
	//			Serial.puts_ln(USART1,"Write a text data to file error\n\r");
	//		} else {
	//			Serial.puts_ln(USART1,"%u bytes written\n\r");
	//		}
	//		Serial.puts_ln(USART1,"Close the file\n\r");
	//		ret = f_close(&file);
	//		if (ret) {
	//			Serial.puts_ln(USART1,"Close the hello.txt file error\n\r");
	//		}
	//	}
	//	Serial.puts_ln(USART1,"read the file (hello.txt)\n\r");
	//	  ret = f_open(&file, "HELLO.TXT", FA_READ);
	//	  if (ret) {
	//	   Serial.puts_ln(USART1,"open hello.txt file error\n\r");
	//	  } else {
	//	    for (;;) {
	//	      ret = f_read(&file, buff, sizeof(buff), &br);	/* Read a chunk of file */
	//	      if (ret || !br) {
	//	        break;			/* Error or end of file */
	//	      }
	//	      buff[br] = 0;
	//	     Serial.puts_ln(USART1,(char*)buff);
	//	     Serial.puts_ln(USART1,"\n\r");
	//	    }
	//	    if (ret) {
	//	     Serial.puts_ln(USART1,"Read file (hello.txt) error\n\r");
	//	    }
	//
	//	   Serial.puts_ln(USART1,"Close the file (hello.txt)\n\r");
	//	    ret = f_close(&file);
	//	    if (ret) {
	//	     Serial.puts_ln(USART1,"Close the file (hello.txt) error\n\r");
	//	    }
	//	  }

//	Config.read(CONFIG_FOLDER,"/CONFIG/BASE.TXT",READ_MODE_CONFIGFILE,"");
};

int sd::get_file_handle(unsigned char *pathToFile,FIL *file_handle,uint8_t flags){
	unsigned char last_filename_buffer[strlen((char*)pathToFile)];
	return get_file_handle(pathToFile,last_filename_buffer,file_handle,flags);
}

int sd::get_file_handle(unsigned char *msgBuffer,unsigned char *last_file,FIL *file_handle,uint8_t flags){
	/* msgBuffer[0..X] =Filename
	 */
	// gibt den pointer auf die datei . file_handle zurück

	unsigned int start_of_real_filename=0;		// pos of the "FILE"name
	unsigned int length_of_filename=strlen((char*)msgBuffer);
	char filename[13];
	char subdir[13];
	subdir[0]='\0';
	unsigned char subdir_pointer=0;
	Serial.puts_ln(USART1,"Closing file");
//	f_close(file_handle);
	Serial.puts_ln(USART1,"Open Root");
	FRESULT res = f_chdir("/");
	if (res){
		return -4;
	}
	Serial.puts_ln(USART1,"Root open");
	// check filename for subdirs and open them one by one
	for(unsigned int i=0;i<length_of_filename;i++){
		// we have to destinguish between normal chars and the '/'
		if(msgBuffer[i]=='/'){ // open actual subdir
			start_of_real_filename=i+1;
			if(subdir[0]!='\0'){ // check if we have something .. just error prevention
				Serial.puts(USART1,"Opening subdir:");
				Serial.puts_ln(USART1,subdir);
				if(f_chdir(subdir)>0){ // if chdir >0 an error occured, than create it
					Serial.puts_ln(USART1,"Failed");
					if((flags && (FA_OPEN_ALWAYS | FA_CREATE_NEW | FA_CREATE_ALWAYS))==0){
						return -3;
					}
					Serial.puts_ln(USART1,"creating");
					if(f_mkdir(subdir)>0){ // if mkdir>0 an error occured, give up
						Serial.puts_ln(USART1,"Failed!");
						return -2; // could not create it
					}

				}
				Serial.puts_ln(USART1,"passing on");
				subdir[0]='\0';
				subdir_pointer=0;
			}
		} else { // regular char
			subdir[subdir_pointer]=msgBuffer[i];
			if(subdir_pointer<10) subdir_pointer++; // dir should be max 8 chars ..
			subdir[subdir_pointer]='\0';
		}
	}
	Serial.puts(USART1,"check if a is requested file:");

	if(msgBuffer[length_of_filename-1]!='/'){ // if the very last char in the filename NOT equals "/" -. then its a file
		Serial.puts_ln(USART1,"yes");
		// dateinamen auslesen
		for(unsigned int i=start_of_real_filename;i<length_of_filename;i++){
			filename[i-start_of_real_filename]=msgBuffer[i];
			last_file[i-start_of_real_filename]=msgBuffer[i];
			if(i==(length_of_filename-1)){
				filename[i-start_of_real_filename+1]='\0';
				last_file[i-start_of_real_filename+1]='\0';
			};
		};
		Serial.puts(USART1,"Opening file:");
		Serial.puts_ln(USART1,filename);

		// datei oeffnen
		if(f_open(file_handle, filename, flags)!=0){ // open existing file in read and write mode, flags could be FA_OPEN_ALWAYS | FA_WRITE | FA_READ
			return -1;
		}
		Serial.puts(USART1,"Done, returning.");
	}
	return length_of_filename;
}

// status: 0=EOF, 1=FILE, 2=FOLDER
int16_t sd::ls_item(uint8_t* path,uint8_t* name,uint16_t item, uint16_t* size){
	//	if(path[strlen((const char*)path)-1]!=(uint8_t)'/'){
	//		path[strlen((const char*)path)]=(uint8_t)'/';
	//	}
	//
	//	FRESULT res;
	//	FILINFO fno;
	//	DIR dir;
	//	int i,count=0;
	//
	//	Serial.puts(USART1, "Open dir: ");
	//	Serial.puts(USART1, (char*)path);
	//	res = f_opendir(&dir, (const char*)path);                       /* Open the directory */
	//	Serial.puts(USART1, " done!\n\r");
	//	if (res == FR_OK) {
	//		i = strlen((const char*)path);
	//		for (;;) {
	//			res = f_readdir(&dir, &fno);                   /* Read a directory item */
	//			if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
	//			if (fno.fname[0] == '.') continue;             /* Ignore dot entry */
	//			if(count==item){
	//				// looping needed? TODO
	////				*name=(uint8_t)fno.fname;
	//				*size=fno.fsize;
	//
	//				if (fno.fattrib & AM_DIR) {                    /* It is a directory */
	//					Serial.puts(USART1, "iteration with!\r\n"); // TODO remove
	//					Serial.puts(USART1, (char*)path); // TODO remove
	//					f_closedir(&dir);
	//					return 2; // folder
	//
	//				} else {                                       /* It is a file. */
	//					f_closedir(&dir);
	//					return 1; // file
	//				}
	//			} else {
	//				count++;
	//			}
	//		}
	//		f_closedir(&dir);
	//		return 0;
	//	};
	//	return -1;
};
