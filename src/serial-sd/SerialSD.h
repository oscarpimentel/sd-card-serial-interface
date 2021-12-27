/*
*/
#ifndef SerialSD_h
#define SerialSD_h

#include "Arduino.h"
#include <SD.h>

#define __DEBUG__
#ifdef __DEBUG__
#define DEBUG(...) Serial.print(__VA_ARGS__)
#else
#define DEBUG(...)
#endif

#define DELIMITER ':'

#define STATE_IDLE -1
#define STATE_LOOP_OK 0
#define STATE_SD_ERROR 1

//############################################################

class SerialSD
{
	public:
		// begins
		SerialSD(int _sd_pin, int _sd_write_ledpin);
		void begin();
		void begin_sd();
		void begin_ledpins(int ledpin_state);

		// info
		void print_info();

		// state
		void change_state(int new_state);
		void report_state();
		void reset_loop_counter();

		// loop
		void loop();
	private:
		int sd_pin;
		int sd_write_ledpin;

		int state;
		long loop_counter;
		File file;
};

#endif