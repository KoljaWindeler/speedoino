#include <inttypes.h>
#include <avr/pgmspace.h>

#ifndef z_code_H
#define z_code_H


static uint8_t speedoino_data[] = {
		// @333 'E' (5 pixels wide)
		0xF8, // #####
		0x80, // #
		0xE0, // ###
		0x80, // #
		0x80, // #
		0xF8, // #####

		// @324 'D' (5 pixels wide)
		0xF0, // ####
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0xF0, // ####


		// @369 'I' (5 pixels wide)
		0xF8, // #####
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0xF8, // #####


		// @414 'N' (5 pixels wide)
		0x88, // #   #
		0xC8, // ##  #
		0xA8, // # # #
		0x98, // #  ##
		0x88, // #   #
		0x88, // #   #

		// @423 'O' (5 pixels wide)
		0xF8, // #####
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0xF8, // #####

		// @459 'S' (5 pixels wide)
		0xF8, // #####
		0x80, // #
		0xF8, // #####
		0x08, //     #
		0x08, //     #
		0xF8, // #####

		// @432 'P' (5 pixels wide)
		0xF0, // ####
		0x88, // #   #
		0xF8, // #####
		0x80, // #
		0x80, // #
		0x80  // #
};
#endif
