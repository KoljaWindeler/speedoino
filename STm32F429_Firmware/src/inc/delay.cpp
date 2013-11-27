#include "global.h"

extern "C" {

void _delay_us(__IO uint32_t nCount) {
	nCount*=21;
	while(nCount--) {
	}
}

void _delay_ms(__IO uint32_t nCount) {
	_delay_us(1000*nCount);
};
};
