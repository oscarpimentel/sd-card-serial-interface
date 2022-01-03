#include "src/serial-sd/SerialSD.h"
// #include <SPI.h> // causes bug with SdFat
#include <SdFat.h>
SdFat sd_card;

#define BAUD_RATE 500000
#define SD_PIN 10
#define SD_WRITE_LEDPIN 4
#define SD_IN_LEDPIN 5
#define SD_DONE_LEDPIN 6
#define SD_IN_BUTTONPIN 7

SerialSD serial_sd(&sd_card, SD_PIN, SD_WRITE_LEDPIN, SD_IN_LEDPIN, SD_DONE_LEDPIN, SD_IN_BUTTONPIN);

//############################################################

void setup(){
	Serial.begin(BAUD_RATE);
	serial_sd.begin();
}

//############################################################

void loop(){
	serial_sd.loop();
}