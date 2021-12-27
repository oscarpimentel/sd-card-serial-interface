/*
*/
#ifndef SerialSD_h
#define SerialSD_h

#include "Arduino.h"
#include <SD.h>
#include <Ledpin.h>

// #define __DEBUG__
#ifdef __DEBUG__
#define DEBUG(...) Serial.print(__VA_ARGS__)
#else
#define DEBUG(...)
#endif

#define DELIMITER ':'

#define STATE_IDLE -1
#define STATE_WAITING_FOR_SD_IN 0
#define STATE_WAITING_SERIAL_BUFFERS 1
#define STATE_WAITING_FOR_SD_OUT 2
#define STATE_SD_ERROR 3

//############################################################

class SerialSD
{
	public:
		SerialSD(int _sd_pin, int _sd_write_ledpin, int _sd_waiting_ledpin);
		SerialSD(void); // empty constructor

		// begins
		void begin();
		void begin_sd();
		void begin_ledpins();

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
		Ledpin sd_write_ledpin;
		Ledpin sd_waiting_ledpin;

		int state;
		long loop_counter;
		File file;
		String record_filedir;
};

#endif