/*

	Console code - Scott Penrose <scottp@dd.com.au>

	Map console to multiple outputs. Use Printf and a buffer. Automatically
	support PROGMEM (Flash memory), String object and char pointers.

	TODO:
		* Turn this into a class

			Console console(Serial);
			console.printf(F("Example %04d\n"), 42);
			File log = SD.open('logfile.txt');
			console.addStream(log);
			Bluetooth bt(9600);
			console.addStream(bt);

	Console console();
	void setup() {
		Serial.begin(115200);
		console.add(Serial);

		console.printf(F("Hello world"));

		* Rename to something better than console

		* Release open source on github Sh3d

		* Add print from SD card - ability to get and print data form a SD card out

		* Add print from PROGMEM arrays

		* Add print from SPIFlash

		* Incorporate Logging levels - so this becomes a logger as well
			console.info(F("This is a info"));
			console.warn(F("This is a warning"));
			console.error(F("This is an error"));
			console.level(CONSOLE_WARN);	// Warnings and above

		* Test on:	
			Moteino - Used here and developed for this node
			Teensy - for BluePower and other modules (including multiple serial and logging and Modem)
			LinkitOne - including logging to SD Card

*/
#include "console.h"
#include "config.h"

#define BUFFER_SIZE 150
char printBuffer[BUFFER_SIZE];


// Just a string
void consolePrint(char *out) {
	CONSOLE.print(out);
	#ifdef Serial0
	Serial0.print(out);
	#endif
	// TODO optional logging etc
	// TODO optional multiple serial port output
}

// printf support
void consolePrintf(char* out, ...) {
	va_list argptr;
	va_start(argptr, out);
	vsnprintf(printBuffer, BUFFER_SIZE, out, argptr);
	va_end(argptr);
	consolePrint(printBuffer);
}

// Convert FLASH String to String
void consolePrintf(const __FlashStringHelper *out, ... ){
	va_list argptr;
	va_start(argptr, out);
	vsnprintf_P(printBuffer, BUFFER_SIZE, (const char *)out, argptr);
	va_end(argptr);
	consolePrint(printBuffer);
}

// Convert STRING to Char
void consolePrintf(String& out, ...) {
	char strBuffer[BUFFER_SIZE];
	out.toCharArray(strBuffer, BUFFER_SIZE);

	va_list argptr;
	va_start(argptr, out);
	vsnprintf(printBuffer, BUFFER_SIZE, strBuffer, argptr);
	va_end(argptr);

	consolePrintf(printBuffer, argptr);
}

