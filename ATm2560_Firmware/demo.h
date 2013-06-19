#ifndef DEMO_H_
#define DEMO_H_

class speedo_demo{
public:

private:
	void get_dz(const char *data);
};

#ifdef DEMO_MODE
extern demo* pDemo;
#endif

#endif
