/**
 ******************************************************************************
 * @file      	LibraryHacks.cpp
 * @author    	Tecnologic86
 * @version   	V0.0.0
 * @date      	03.07.2012 15:08:09
 * @project	ThunderCryer
 ******************************************************************************
 */



/*
 * LibraryHacks.cpp  *
 *  Created on: 23 Jan 2011  *
 *  Author: Andy  */

#include <cstdlib>
#include <sys/types.h>

/*
 * The default pulls in 70K of garbage
 */
namespace __gnu_cxx {
void __verbose_terminate_handler() {
	for(;;);
}
}

/*
 * The default pulls in about 12K of garbage
 */
extern "C" void __cxa_pure_virtual() {
	for(;;);
}

/*
 * Implement C++ new/delete operators using the heap
 */
void *operator new(size_t size) {
	return malloc(size);
}

void *operator new[](size_t size) {
	return malloc(size);
}

void operator delete(void *p) {
	free(p);
}

void operator delete[](void *p) {
	free(p);
}

/*
 * sbrk function for getting space for malloc and friends
 */
extern int  _end;

extern "C" {
caddr_t _sbrk ( int incr ) {
	static unsigned char *heap = NULL;
	unsigned char *prev_heap;

	if (heap == NULL) {
		heap = (unsigned char *)&_end;
	}     prev_heap = heap;

	/* check removed to show basic approach */
	heap += incr;
	return (caddr_t) prev_heap;
}
}

