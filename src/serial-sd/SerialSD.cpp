/*
*/
#include "Arduino.h"
#include "SerialSD.h"

//############################################################

String split_string(String data, char delimiter, int index){
	// https://stackoverflow.com/questions/9072320/split-string-into-string-array
	int found = 0;
	int strIndex[] = {0, -1};
	int maxIndex = data.length()-1;

	for (int i=0; i<=maxIndex && found<=index; i++){
		if (data.charAt(i)==delimiter || i==maxIndex){
			found++;
			strIndex[0] = strIndex[1]+1;
			strIndex[1] = (i == maxIndex) ? i+1 : i;
		}
	}
	return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

FlagArg get_string_flag_arg(String data){
	struct FlagArg flag_arg;
	data.replace("\r", "");
	data.replace("\n", "");
	flag_arg.flag = data.substring(0, 3);
	flag_arg.arg = data.substring(4);
	return flag_arg;
}

//############################################################

SerialSD::SerialSD(int _sd_pin, int _sd_write_ledpin, int _sd_in_ledpin, int _sd_done_ledpin, int _sd_in_buttonpin){
	sd_pin = _sd_pin;
	sd_write_ledpin = Ledpin(_sd_write_ledpin);
	sd_in_ledpin = Ledpin(_sd_in_ledpin);
	sd_done_ledpin = Ledpin(_sd_done_ledpin);
	sd_in_buttonpin = Buttonpin(_sd_in_buttonpin, true);
}

SerialSD::SerialSD(void){}

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
	while(!SD.begin(sd_pin)){
		DEBUG(".");
	}
	DEBUGLN();
}

//############################################################

void SerialSD::print_info(){
	// Serial.print("state="); Serial.println(state);
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
			if (flag_arg.flag=="--o"){
				record_filedir = flag_arg.arg;
				bool removed = SD.remove(record_filedir);
				file = SD.open(record_filedir, FILE_WRITE);
				Serial.println(removed);

			}else if (flag_arg.flag=="--w"){
				record_filedir = split_string(flag_arg.arg, DELIMITER, 0);
				String buffer_text = split_string(flag_arg.arg, DELIMITER, 1);
				DEBUGLN(record_filedir);
				DEBUGLN(buffer_text);
				if (file){
					sd_write_ledpin.high();
					file.println(buffer_text); // write buffer
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