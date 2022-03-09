/*
*/
#ifndef SerialSD_h
#define SerialSD_h

#include "Arduino.h"
#include <Ledpin.h>
#include <Buttonpin.h>
// #include <SPI.h> // causes bug with SdFat?
#include <SdFat.h>

// #define __DEBUG__
#ifdef __DEBUG__
#define DEBUG(...) Serial.print(__VA_ARGS__)
#define DEBUGLN(...) Serial.println(__VA_ARGS__)
#else
#define DEBUG(...)
#define DEBUGLN(...)
#endif

#define STATE_IDLE -1
#define STATE_WAITING_FOR_SD_IN 0
#define STATE_WAITING_FOR_SD_OUT 1
#define STATE_WAITING_SERIAL_BUFFERS 2
#define STATE_WAITING_FOR_SD_OUT_AFTER_COPY 3
#define STATE_SD_ERROR 4

//############################################################

struct FlagArg{
	String flag = "";
	String arg = "";
};

//############################################################

class SerialSD{
	public:
		SerialSD(SdFat* _sd_card, int _sd_pin, int _sd_write_ledpin, int _sd_in_ledpin, int _sd_done_ledpin, int _sd_in_buttonpin);
		SerialSD(void); // empty constructor
		void begin();

		// begins
		void begin_sd();
		void begin_ledpins();

		// info
		void print_info();

		// state
		void change_state(int new_state);
		void report_state();
		void reset_loop_counter(unsigned long new_loop_counter_value=0);
		void add_loop_counter();
		unsigned long get_loop_counter();

		// loop
		void loop();
	private:
		SdFat* sd_card;
		int sd_pin;
		Ledpin sd_write_ledpin;
		Ledpin sd_in_ledpin;
		Ledpin sd_done_ledpin;
		Buttonpin sd_in_buttonpin;

		int state;
		unsigned long loop_counter;
		SdFile file;
		String serial_str;
		char record_filedir[20];
};

#endif