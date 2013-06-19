#ifndef DEMO_H_
#define DEMO_H_
#ifdef DEMO_MODE

class speedo_demo{
public:
	speedo_demo();
	~speedo_demo();
	unsigned int get_RPM();
	unsigned int get_speed();
	int get_water_temperature();
	int get_oil_temperature();
private:
	void get_dz(const char *data);
	unsigned int timing_sec[6];
};


extern speedo_demo* pDemo;
#endif

#endif
