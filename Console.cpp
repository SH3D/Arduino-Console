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
#include <Console.h>

// Just a string
void Console::print(char *out) {
	for(byte i = 0; i < CONSOLE_STREAMS; i++) 
		if (streams[i])
			streams[i]->print(out);
}

// printf support
void Console::printf(char* out, ...) {
	va_list argptr;
	va_start(argptr, out);
	vsnprintf(printBuffer, CONSOLE_BUFFER_SIZE, out, argptr);
	va_end(argptr);
	print(printBuffer);
}

// Convert FLASH String to String
void Console::printf(const __FlashStringHelper *out, ... ){
	va_list argptr;
	va_start(argptr, out);
	vsnprintf_P(printBuffer, CONSOLE_BUFFER_SIZE, (const char *)out, argptr);
	va_end(argptr);
	print(printBuffer);
}

// Convert STRING to Char
void Console::printf(String& out, ...) {
	// char strBuffer[CONSOLE_BUFFER_SIZE];
	// out.toCharArray(strBuffer, CONSOLE_BUFFER_SIZE);

	va_list argptr;
	va_start(argptr, out);
	vsnprintf(printBuffer, CONSOLE_BUFFER_SIZE, out.c_str(), argptr);
	va_end(argptr);

	print(printBuffer);
}

// printf support
void Console::info(char* out, ...) {
	va_list argptr;
	va_start(argptr, out);
	vsnprintf(printBuffer, CONSOLE_BUFFER_SIZE, out, argptr);
	va_end(argptr);
	print(printBuffer);
}

// Convert FLASH String to String
void Console::info(const __FlashStringHelper *out, ... ){
	va_list argptr;
	va_start(argptr, out);
	vsnprintf_P(printBuffer, CONSOLE_BUFFER_SIZE, (const char *)out, argptr);
	va_end(argptr);
	print(printBuffer);
}

// Convert STRING to Char
void Console::info(String& out, ...) {
	va_list argptr;
	va_start(argptr, out);
	vsnprintf(printBuffer, CONSOLE_BUFFER_SIZE, out.c_str(), argptr);
	va_end(argptr);
	print(printBuffer);
}

// printf support
void Console::warn(char* out, ...) {
	va_list argptr;
	va_start(argptr, out);
	vsnprintf(printBuffer, CONSOLE_BUFFER_SIZE, out, argptr);
	va_end(argptr);
	print(printBuffer);
}

// Convert FLASH String to String
void Console::warn(const __FlashStringHelper *out, ... ){
	va_list argptr;
	va_start(argptr, out);
	vsnprintf_P(printBuffer, CONSOLE_BUFFER_SIZE, (const char *)out, argptr);
	va_end(argptr);
	print(printBuffer);
}

// Convert STRING to Char
void Console::warn(String& out, ...) {
	va_list argptr;
	va_start(argptr, out);
	vsnprintf(printBuffer, CONSOLE_BUFFER_SIZE, out.c_str(), argptr);
	va_end(argptr);
	print(printBuffer);
}

// printf support
void Console::error(char* out, ...) {
	va_list argptr;
	va_start(argptr, out);
	vsnprintf(printBuffer, CONSOLE_BUFFER_SIZE, out, argptr);
	va_end(argptr);
	print(printBuffer);
}

// Convert FLASH String to String
void Console::error(const __FlashStringHelper *out, ... ){
	va_list argptr;
	va_start(argptr, out);
	vsnprintf_P(printBuffer, CONSOLE_BUFFER_SIZE, (const char *)out, argptr);
	va_end(argptr);
	print(printBuffer);
}

// Convert STRING to Char
void Console::error(String& out, ...) {
	va_list argptr;
	va_start(argptr, out);
	vsnprintf(printBuffer, CONSOLE_BUFFER_SIZE, out.c_str(), argptr);
	va_end(argptr);
	print(printBuffer);
}

/* HOW TO easily share 
void Console::info(char* out, ...) {
	if (level >= CONSOLE_LEVEL_INFO) {
		printf(F("INFO: ");
}
void Console::info(const __FlashStringHelper *out, ... ){
}
void Console::info(String& out, ...) {
}
*/

void Console::loop() {
	// Reset cmd
	if (cmd) {
		cmdCount = 0;
		cmd = NULL;
	}

	for(byte i = 0; i < CONSOLE_STREAMS; i++) {
		if (streams[i] && streams[i]->available()) 
			processCmd(streams[i]->read());
	}
}

void Console::processCmd(char in) {
	if ( (in != '\n') && (in != '\r') ) {
		cmdBuffer[cmdCount] = in;
		cmdCount++;
		cmdBuffer[cmdCount] = NULL;
		if (cmdCount >= CONSOLE_BUFFER_SIZE) {
			cmdCount = 0;
			error(F("# CONSOLE: Error, command too long\r\n"));
		}
	}
	else if (cmdCount > 0) {
		// Add optional ending, or even CRC
		if (cmdBuffer[cmdCount - 1] != ';') {
			warn("# Warning - Buffer cleared, must end in trailing ;\r\n");
			cmdCount = 0;
		}
		else {
			cmd = cmdBuffer[0];
			val = atoi(&cmdBuffer[1]);
		}
	}
}

bool Console::available(){
	return cmd != NULL;
}

bool Console::add(Stream *s) {
	for(byte i = 0; i < CONSOLE_STREAMS; i++) {
		if (streams[i] == s) {
			return true;
		}
		else if (! streams[i]) {
			streams[i] = s;
			return true;
		}
	}
	return false;
}

bool Console::remove(Stream *s) {
	for(byte i = 0; i < CONSOLE_STREAMS; i++) {
		if (streams[i] == s) {
			streams[i] = NULL;
			// TODO consider copy back to reduce loops
			return true;
		}
	}
	return false;
}

void Console::setLevel(byte l) {
	level = l;
}

Console console;

