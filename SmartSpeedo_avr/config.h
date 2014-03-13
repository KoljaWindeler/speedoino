/*
 * config.h
 *
 *  Created on: 01.06.2011
 *      Author: kolja
 */

#ifndef CONFIG_H_
#define CONFIG_H_


// config Container
class configuration{
/*********************************** config erzeugen *********************************/
public:
	configuration(void);
	~configuration();
	int get_hw_version();
	void ram_info();
private:
	char hw_version;
};
extern configuration *pConfig;
///////////// CONFIG /////////////////////////

#endif /* CONFIG_H_ */
