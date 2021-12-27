#include "src/serial-sd/SerialSD.h"  

#define BAUD_RATE 500000
#define SD_PIN 10
#define SD_WRITE_LEDPIN 4

SerialSD serial_sd(SD_PIN, SD_WRITE_LEDPIN);

//############################################################

void setup(){
	Serial.begin(BAUD_RATE);
	serial_sd.begin();
}

//############################################################

void loop(){
	serial_sd.loop();
	// serial_sd.print_info();
}