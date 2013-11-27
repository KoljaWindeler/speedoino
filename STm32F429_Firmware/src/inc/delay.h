/*
 * delay.h
 *
 *  Created on: 21.10.2013
 *      Author: jkw
 */

#ifndef DELAY_H_
#define DELAY_H_

#define STEPS_PER_US 2200L
void _delay_ms(volatile uint32_t nCount);
void _delay_us(volatile uint32_t nCount);



#endif /* DELAY_H_ */
