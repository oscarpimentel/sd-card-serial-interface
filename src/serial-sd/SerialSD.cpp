/*
*/
#include "Arduino.h"
#include "SerialSD.h"

//############################################################

String split_string(String data, char separator, int index){
	// https://stackoverflow.com/questions/9072320/split-string-into-string-array
	int found = 0;
	int strIndex[] = {0, -1};
	int maxIndex = data.length()-1;

	for (int i=0; i<=maxIndex && found<=index; i++){
		if (data.charAt(i)==separator || i==maxIndex){
			found++;
			strIndex[0] = strIndex[1]+1;
			strIndex[1] = (i == maxIndex) ? i+1 : i;
		}
	}
	return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

//############################################################

SerialSD::SerialSD(int _sd_pin, int _sd_write_ledpin){
	sd_pin = _sd_pin;
	sd_write_ledpin = _sd_write_ledpin;
}

void SerialSD::begin(){
	change_state(STATE_IDLE);
	loop_counter = 0;
	begin_ledpins(HIGH);
	begin_sd();
	begin_ledpins(LOW);
	change_state(STATE_LOOP_OK);
}

void SerialSD::begin_ledpins(int ledpin_state){
	pinMode(sd_write_ledpin, OUTPUT); digitalWrite(sd_write_ledpin, ledpin_state);
}

void SerialSD::begin_sd(){
	DEBUG("begining SD card");
	pinMode(sd_pin, OUTPUT); digitalWrite(sd_pin, HIGH);
	while(!SD.begin(sd_pin)){
		DEBUG(".");
	}
	DEBUG('\n');
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
	print_info();
}

void SerialSD::reset_loop_counter(){
	loop_counter = 0;
}


//############################################################

void SerialSD::loop(){
	loop_counter += 1;
	if (state==STATE_LOOP_OK){ // STATE
		if (Serial.available()){
			String serial_str = Serial.readStringUntil('\n');
			serial_str.replace("\r", "");
			serial_str.replace("\n", "");
			String flag = serial_str.substring(0, 3);
			if (flag=="--r"){
				String record_filedir = serial_str.substring(4);
				bool removed = SD.remove(record_filedir);
				Serial.println((int)removed);

			}else if (flag=="--w"){
				String msj = serial_str.substring(4);
				String record_filedir = split_string(msj, DELIMITER, 0);
				String buffer_text = split_string(msj, DELIMITER, 1);
				file = SD.open(record_filedir, FILE_WRITE);

				if (file){
					digitalWrite(sd_write_ledpin, HIGH);
					file.println(buffer_text); // write buffer
					file.close(); // close the file
					Serial.println('1');
					digitalWrite(sd_write_ledpin, LOW);
				}else{
					change_state(STATE_SD_ERROR);
				}
			}
		}
	}else if(state==STATE_SD_ERROR){ // STATE
		begin_sd();
		change_state(STATE_LOOP_OK);
	}else{
		;
	}
}