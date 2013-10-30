
#ifndef __MILLIS_H
#define __MILLIS_H

class timing{

private:
	volatile uint32_t millis_value;
public:
	timing();
	void inc();
	void init();
	uint32_t get();

};
extern timing Millis;
#endif
