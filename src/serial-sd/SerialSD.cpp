/*
*/
#include "Arduino.h"
#include "SerialSD.h"

//############################################################

FlagArg get_string_flag_arg(String data){
	struct FlagArg flag_arg;
	data.replace("\r", "");
	data.replace("\n", "");
	flag_arg.flag = data.substring(0, 3);
	flag_arg.arg = data.substring(4);
	return flag_arg;
}

//############################################################

SerialSD::SerialSD(SdFat* _sd_card, int _sd_pin, int _sd_write_ledpin, int _sd_in_ledpin, int _sd_done_ledpin, int _sd_in_buttonpin){
	sd_card = _sd_card;
	sd_pin = _sd_pin;
	sd_write_ledpin = Ledpin(_sd_write_ledpin);
	sd_in_ledpin = Ledpin(_sd_in_ledpin);
	sd_done_ledpin = Ledpin(_sd_done_ledpin);
	sd_in_buttonpin = Buttonpin(_sd_in_buttonpin, true);
}

SerialSD::SerialSD(void){
	;
}

//############################################################

void SerialSD::begin(){
	change_state(STATE_IDLE);
	loop_counter = 0;
	pinMode(sd_pin, OUTPUT); digitalWrite(sd_pin, HIGH);
	begin_ledpins();
	sd_in_buttonpin.begin();
	change_state(STATE_WAITING_FOR_SD_IN);
}

void SerialSD::begin_ledpins(){
	sd_write_ledpin.begin();
	sd_in_ledpin.begin();
	sd_done_ledpin.begin();
}

void SerialSD::begin_sd(){
	DEBUG("begining SD card");
	pinMode(sd_pin, OUTPUT); digitalWrite(sd_pin, HIGH);
	while(!sd_card->begin(sd_pin, SPI_FULL_SPEED)){ // SPI_FULL_SPEED SPI_HALF_SPEED
		DEBUG(".");
		// sd_card->initErrorHalt();
	}
	DEBUGLN();
}

//############################################################

void SerialSD::print_info(){
	// DEBUG("state="); DEBUGLN(state);
	// print_sd_buffer();
}

//############################################################

void SerialSD::change_state(int new_state){
	state = new_state;
	report_state();
	reset_loop_counter();
}

void SerialSD::report_state(){
	DEBUGLN(state);
	print_info();
}

void SerialSD::reset_loop_counter(unsigned long new_loop_counter_value=0){
	loop_counter = new_loop_counter_value;
}

void SerialSD::add_loop_counter(){
	loop_counter += 1;
	// delay(10);
}

unsigned long SerialSD::get_loop_counter(){
	return loop_counter;
}
//############################################################

void SerialSD::loop(){
	add_loop_counter();
	if (state==STATE_WAITING_FOR_SD_IN){ // STATE
		sd_in_ledpin.low();
		if(sd_in_buttonpin.is_on()){
			change_state(STATE_WAITING_FOR_SD_OUT);
		}

	}else if(state==STATE_WAITING_FOR_SD_OUT){ // STATE
		sd_in_ledpin.high();
		if (!sd_in_buttonpin.is_on()){
			change_state(STATE_WAITING_FOR_SD_IN);
		}
		if (Serial.available()){
			serial_str = Serial.readStringUntil('\n');
			FlagArg flag_arg = get_string_flag_arg(serial_str);
			if (flag_arg.flag=="--1"){
				begin_sd();
				Serial.println(1);
				change_state(STATE_WAITING_SERIAL_BUFFERS);
			}
		}

	}else if(state==STATE_WAITING_SERIAL_BUFFERS){ // STATE
		sd_in_ledpin.high();
		if (Serial.available()){
			serial_str = Serial.readStringUntil('\n');
			FlagArg flag_arg = get_string_flag_arg(serial_str);
			DEBUGLN(flag_arg.flag);
			DEBUGLN(flag_arg.arg);
			if (flag_arg.flag=="--o"){
				flag_arg.arg.toCharArray(record_filedir, sizeof(record_filedir));
				file.open(record_filedir, O_RDWR | O_CREAT | O_AT_END); // open the file
				file.remove();
				file.open(record_filedir, O_RDWR | O_CREAT | O_AT_END); // open the file
				Serial.println(1);

			}else if (flag_arg.flag=="--w"){
				if (file){
					sd_write_ledpin.high();
					file.println(flag_arg.arg); // write buffer
					Serial.println(1);
					sd_write_ledpin.low();
				}else{
					change_state(STATE_SD_ERROR);
				}

			}else if (flag_arg.flag=="--c"){
				file.close(); // close the file
				Serial.println(1);

			}else if (flag_arg.flag=="--0"){
				change_state(STATE_WAITING_FOR_SD_OUT_AFTER_COPY);
				Serial.println(1);
			}
		}

	}else if(state==STATE_WAITING_FOR_SD_OUT_AFTER_COPY){ // STATE
		sd_in_ledpin.high();
		sd_done_ledpin.high();
		if (!sd_in_buttonpin.is_on()){
			sd_done_ledpin.low();
			change_state(STATE_WAITING_FOR_SD_IN);
		}

	}else if(state==STATE_SD_ERROR){ // STATE
		begin_sd();
		change_state(STATE_WAITING_FOR_SD_IN);

	}else{
		;
	}
}