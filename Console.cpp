/*

	Console code - Scott Penrose <scottp@dd.com.au>

	Map console to multiple outputs. Use Printf and a buffer. Automatically
	support PROGMEM (Flash memory), String object and char pointers.

		* XXX This should be callbacks
			* NMEA Strings updated - do This
			* Etc

		* XXX Binary is almost certainly out by one start/end or both

		* XXX Write test code that sends streams:
			* NMEA - obvious one
			* @CSV
			* !Binary

	Console console();
	void setup() {
		SerialConsole.begin(115200);
		console.begin();
		console.add(&SerialConsole);

		console.printf(F("Hello world"));

		* Rename to something better than console
		* Release open source on github Sh3d
		* Add print from SD card - ability to get and print data form a SD card out
		* Add print from PROGMEM arrays
		* Add print from SPIFlash

	NOTE on NMEA devices
		* NMEA is always processed from incoming data on the consoles
		* If a second "TinyGPS++" is required, you can addNmea stream instead
		* OR even better, just add a stream as add(&HWSerial, CONSOLE_DIRECTION_INPUT);

	Restructure to simpler methods:
		* BEFORE
			= Sketch size 15378
		* AFTER
			= Sketch size 15046 (saved 340 bytes)

	Restructure to error numbers:
		* Sh3dNodeErrors
			* console.error(n)
			* console.safeError(n)
			* uint16_t - 01..65 modules split, 1000 message per module
			* Could we add an optional long?
*/
#include "Arduino.h"
#include <WII5Sh3dConsole.h>
#include <TimeLib.h>
#include <MemoryFree.h>
// printf support
// Just a string
void WII5Sh3dConsole::print(char *out) {
	for(byte i = 0; i < CONSOLE_STREAMS; i++) {
		if (
			streams[i]
			&& (
				(streamsDirection[i] == CONSOLE_DIRECTION_BOTH)
				|| (streamsDirection[i] == CONSOLE_DIRECTION_OUTPUT)
			)
		) {
			streams[i]->print(out);
			#ifdef CONSOLE_DEBUG_FLUSH
			streams[i]->flush();
			#endif
		}
	}
}

// sprintf - STREAM printf
void WII5Sh3dConsole::sprintf(Stream *s, const __FlashStringHelper *out, ... ) {
	if (!check()) return;
	va_list argptr;
	va_start(argptr, out);
	VSNPRINTF(printBuffer, bufferMax, (const char *)out, argptr);
	va_end(argptr);
	s->print(printBuffer);
}

// printf support
void WII5Sh3dConsole::printf(char* out, ...) {
	if (!check()) return;
	va_list argptr;
	va_start(argptr, out);
	vsnprintf(printBuffer, bufferMax, out, argptr);
	va_end(argptr);
	print(printBuffer);
}

// Convert FLASH String to String
void WII5Sh3dConsole::printf(const __FlashStringHelper *out, ... ){
	if (!check()) return;
	va_list argptr;
	va_start(argptr, out);
	VSNPRINTF(printBuffer, bufferMax, (const char *)out, argptr);
	va_end(argptr);
	print(printBuffer);
}

// TODO Move to Sh3dNodeErrors
void WII5Sh3dConsole::printPrefix(uint8_t level) {
	switch(level) {
		case LOG_INFO:
			printf(F(CONSOLE_PRE_INFO));
			break;
		case LOG_WARN:
			printf(F(CONSOLE_PRE_WARN));
			break;
		case LOG_ERROR:
			printf(F(CONSOLE_PRE_ERROR));
			break;
		case LOG_DEBUG:
			printf(F(CONSOLE_PRE_DEBUG));
			break;
		case LOG_FATAL:
			printf(F(CONSOLE_PRE_FATAL));
			break;
		default:
			// Do we need unknown?
			break;
	}
	printDateTime();
	printf(F(CONSOLE_POST_TIME));
}

// printf support
void WII5Sh3dConsole::log(uint8_t l, char* out, ...) {
	if (level < l) return;
	if (!check()) return;
	printPrefix(l);
	va_list argptr;
	va_start(argptr, out);
	vsnprintf(printBuffer, bufferMax, out, argptr);
	va_end(argptr);
	print(printBuffer);
	printNewLine();
}

// Convert FLASH String to String
void WII5Sh3dConsole::log(uint8_t l, const __FlashStringHelper *out, ... ){
	if (level < l) return;
	if (!check()) return;
	printPrefix(l);
	va_list argptr;
	va_start(argptr, out);
	VSNPRINTF(printBuffer, bufferMax, (const char *)out, argptr);
	va_end(argptr);
	print(printBuffer);
	printNewLine();
}

// TODO - Compile time option to remove unused entries - e.g We may not support String
void WII5Sh3dConsole::safeLog(uint8_t l, char* out, ...) {
	if (!check()) return;
	printPrefix(l);
	va_list argptr;
	va_start(argptr, out);
	vsnprintf(printBuffer, bufferMax, out, argptr);
	va_end(argptr);
	print(printBuffer);
	printNewLine();
	flush();
}

void WII5Sh3dConsole::safeLog(uint8_t l, const __FlashStringHelper *out, ... ){
	if (!check()) return;
	printPrefix(l);
	va_list argptr;
	va_start(argptr, out);
	VSNPRINTF(printBuffer, bufferMax, (const char *)out, argptr);
	va_end(argptr);
	print(printBuffer);
	printNewLine();
	flush();
}

void WII5Sh3dConsole::printDateTime(uint32_t t) {
	if (t == 0) {
		if (!displayDateTime) return;	// Automatic mode
		#ifdef _Time_h
		t = now();
		#endif
	}
	//	uint16_t - date.year,
	//	uint8_t - date.month, date.day,
	//	uint8_t - time.hour, time.minute, time.second
	sprintf_P(printBuffer, PSTR("%04d-%02d-%02dT%02d:%02d:%02d"),
		year(t),
		int(month(t)),
		int(day(t)),
		int(hour(t)),
		int(minute(t)),
		int(second(t))
	);
	print(printBuffer);
}

void WII5Sh3dConsole::loop() {
	// Reset cmd
	if (cmd) {
		cmdCount = 0;
		csv_count = 0;
		csv_last = 0;
		cmdType = 0;
		cmd = '\0';
	}
	// Reset Binary
  if (binAvailable) {
    binAvailable = false;
    binReceiveCount = 0;
  }

	for(byte i = 0; i < CONSOLE_STREAMS; i++) {
		// NOTE - while important, but can be too much - limit?
		if (
			(streamsDirection[i] == CONSOLE_DIRECTION_BOTH)
			|| (streamsDirection[i] == CONSOLE_DIRECTION_INPUT)
		) {
			while (streams[i] && streams[i]->available())
				processCmd(streams[i]->read());
		}
	}
}

void WII5Sh3dConsole::processCsv() {
	// XXX check for csv_count max and cmdCount max Etc

	// Point to the start of the last strinb before the comma
	csv_str[csv_count] = &cmdBuffer[csv_last];

	// Null the ','
	cmdBuffer[cmdCount] = '\0';

	// Parse to ingeger
	csv_uint32[csv_count] = atol(csv_str[csv_count]);

	// Next character
	csv_last = cmdCount + 1;

	// console.log(LOG_DEBUG, F("CSV found comma last=%d, count=%d, str=%s, lu=%lu"), int(csv_last), int(csv_count), csv_str[csv_count], csv_uint32[csv_count]);

	csv_count++;
}

// processCmd
void WII5Sh3dConsole::processCmd(char in) {
	// DO Timeout
	if ( (cmdCount > 0) && (waitInput > CONSOLE_TIMEOUT) ) {
		printf(F("# CONSOLE: Timeout for new Serial Command. Max time = %d seconds"), int(CONSOLE_TIMEOUT / 1000));
		printNewLine();
		cmdCount = 0;
    binAvailable = false;
    binReceiveCount = 0;
	}

	// BINARY Data mode - keep here
	if (binEnable && (cmdType == CONSOLE_TYPE_BINDATA_ID)) {
    if (cmdCount == 0) {
      printf(F("# FATAL ERROR - Must not be in BINDATA mode with no count, Invalid mode")); printNewLine();
      cmdCount = 0;
      cmdType = 0;
    }

		binBuffer[binReceiveCount] = in;
		binReceiveCount++;

		if (binReceiveCount >= binReceiveExpect) {
			// COMPLETE !
			cmdType = 0;
			cmdCount = 0;
      binAvailable = true;
			return;
		}

		if (binReceiveCount >= binMax) {
			// XXX This should NOT be allowed to happen - since we check below
			safeLog(LOG_FATAL, F("CONSOLE: Binary incoming data too large"));
			cmdCount = 0;
			cmd = '\0';
			val = 0;
			csv_last = 0;
			csv_count = 0;
			binReceiveCount = 0;
		}

		// Rest of function is TEXT processing - leave now
		return;
	}

	// TEXT Mode - NOT a new line, keep data
	if ( (in != '\n') && (in != '\r') ) {
		// First character - see if it is $
		if ( (cmdCount == 0) ) {
			// Reset timeout
			waitInput = 0;

			// $ = NMEA = 1
			if (in == CONSOLE_TYPE_NMEA_C0) {
				cmdType = CONSOLE_TYPE_NMEA_ID;
			}

			// CSV
			else if (in == CONSOLE_TYPE_CSV_C0) {
				csv_last = 1;	// Skip the "@"
				// Start all at Zero
				for (csv_count = 0; csv_count < CONSOLE_TYPE_CSV_LEN; csv_count++) {
					csv_uint32[csv_count] = 0;
				}
				csv_count = 0;
				cmdType = CONSOLE_TYPE_CSV_ID;
				// console.log(LOG_DEBUG, F("CSV START last=%d, count=%d"), int(csv_last), int(csv_count));
			}

			// BINARY
			else if (binEnable && (in == CONSOLE_TYPE_BIN_C0)) {
				cmdType = CONSOLE_TYPE_BIN_ID;
				binReceiveCount = 0;
				binReceiveExpect = 0;
				binReceiveCRC = 0;	// Optional?
			}

			// Else assume standard ending in ";"
			else {
				cmdType = CONSOLE_TYPE_OTHER_ID;
			}
		}

		// Keep cmdBuffer if not NMEA
		if (
			// Keep Other
			cmdType == CONSOLE_TYPE_OTHER_ID		// Keep for external use
			|| cmdType == CONSOLE_TYPE_CSV_ID		// Keep and process on the fly
			|| cmdType == CONSOLE_TYPE_NMEA_ID	// NMEA
			|| cmdType == CONSOLE_TYPE_BIN_ID
		) {

			// TODO test out by one
			if (cmdCount < (consoleMax - 1)) {
				cmdBuffer[cmdCount] = in;

				// XXX ',' should be #define in case we use : or tab or something
				if (cmdType == CONSOLE_TYPE_CSV_ID && in == ',') {
					processCsv();
				}

				cmdCount++;
				cmdBuffer[cmdCount] = '\0';
			}
		}
	}

	// TEXT MODE - New Line - process - NOTE: This may get called twice (\r then \n)
	//	- Once leaving this, the only way for a new command to start
	//	  is to set cmdCount = 0
	else {
		// Do nothing with this
		if (cmdType == CONSOLE_TYPE_IGNORE_ID) {
		}

		// Blank everything
		else if (cmdType == CONSOLE_TYPE_DESTROY_ID) {
			cmdCount = 0;
			cmd = '\0';
			val = 0;
			csv_last = 0;
			csv_count = 0;
		}

		// Binary
		else if (binEnable && (cmdType == CONSOLE_TYPE_BIN_ID)) {
			if (cmdCount < 2) {
				printf(F("# CONSOLE: Error, no size requests for binary data")); printNewLine();
				printf(F("BINARY ERROR: No size")); printNewLine();
				cmdCount = 0;
				cmdType = 0;
			}
			else {
				val = atol(&cmdBuffer[1]);
				if (val >= binMax) {
					printf(F("# CONSOLE: Request Binary larger than buffer")); printNewLine();
					printf(F("BINARY ERROR: Too large")); printNewLine();
					cmd = '\0';
					cmdType = 0;
					cmdCount = 0;
				}
				else {
					printf(F("# CONSOLE: Request Binary size=%d"), binReceiveExpect); printNewLine();
					printf(F("BINARY OK: Ready to send")); printNewLine();
					binReceiveExpect = val;
					binReceiveCount = 0;
					cmdType = CONSOLE_TYPE_BINDATA_ID;
				}
			}
		}

		// Add optional ending, or even CRC
		else if (cmdType == CONSOLE_TYPE_OTHER_ID) {
			if (echo)
				log(LOG_DEBUG, F("CONSOLE Receved: %s"), cmdBuffer);
			// Check we have a buffe?
			if (cmdCount >= (consoleMax - 1)) {
				// printf(F("# CONSOLE overflow: cmd size - c=%d of %d\r\n"), cmdCount, consoleMax);
				printf(F("# CONSOLE cmd overflow"));
				printNewLine();
				cmdCount = 0;
				cmdType = CONSOLE_TYPE_IGNORE_ID;
			}
			else if (cmdBuffer[cmdCount - 1] != ';') {
				printf(F("# CONSOLE: standard input must end in ; count=%d, last=%d"), cmdCount, (uint8_t)cmdBuffer[cmdCount =- 1]);
				printNewLine();
				cmdCount = 0;
				cmdType = CONSOLE_TYPE_IGNORE_ID;
			}
			else {
				cmd = cmdBuffer[0];
				val = atol(&cmdBuffer[1]);
				cmdType = CONSOLE_TYPE_IGNORE_ID;
			}
		}
		else if ( cmdType == CONSOLE_TYPE_NMEA_ID ) {
			// Ignore the NMEA - already processed
			// TODO: Maybe mark the buffer as now a copy of the NMEA for passthrough?
		}
		else if ( cmdType == CONSOLE_TYPE_CSV_ID ) {
			cmdCount++;	// Spin on, so this will be like the "," where we add the null
			processCsv();
			cmd = CONSOLE_TYPE_CSV_C0;
			cmdType = CONSOLE_TYPE_IGNORE_ID;
			if (echo) {
				printPrefix(LOG_DEBUG);
				printf(F("CONSOLE Received (@): "));
        for (i = 0; i < console.getCsvCount(); i++) {
          printf(console.getCsvBuffer(i));
          printf(",");
        }
				printNewLine();
			}
		}
	}
}

bool WII5Sh3dConsole::available(){
	return cmd != '\0';
}

// begin - using External Buffers
void WII5Sh3dConsole::begin(char* buf, uint16_t bufLen, char* cmdBuf, uint16_t cmdLen) {
	// XXX XXX XXX
	echo = true;

	printBuffer = buf;
	bufferMax = bufLen;
	cmdBuffer = cmdBuf;
	consoleMax = cmdLen;
	cmdType = 0;
	setDateTime();
}

// begin - Allocate the buffers and get things going
void WII5Sh3dConsole::begin(uint16_t bufLen, uint16_t cmdLen) {
	if ( (bufLen + cmdLen) >= freeMemory() )
		internalErrorNotEnoughMemory();
	else {
		printBuffer = (char*)malloc(bufLen);
		if (printBuffer == NULL)
			internalErrorNotEnoughMemory();
		else
			bufferMax = bufLen;

		cmdBuffer = (char*)malloc(cmdLen);
		if (cmdBuffer == NULL)
			internalErrorNotEnoughMemory();
		else
			consoleMax = cmdLen;

		cmdType = 0;
		setDateTime();
	}
}

void WII5Sh3dConsole::internalErrorNotEnoughMemory() {
	SerialConsole.println(F("CONSOLE IntErr: failbuf"));
}
void WII5Sh3dConsole::internalErrorNoBufferStream() {
	SerialConsole.println(F("CONSOLE IntErr: buf, stream"));
}

bool WII5Sh3dConsole::check() {
	if (streamCount == 0) {
		internalErrorNoBufferStream();
		return false;
	}
	if (bufferMax == 0) {
		internalErrorNoBufferStream();
		return false;
	}
	return true;
}

void WII5Sh3dConsole::clearCommand() {
	cmdCount = 0;
	csv_count = 0;
	csv_last = 0;
	cmdType = 0;
	cmd = '\0';
}

void WII5Sh3dConsole::clearCallback() {
	callback_default = false;
	callback = NULL;
}
void WII5Sh3dConsole::setCallbackFunction(CallbackFunction f) {
	callback_default = false;
	callback = f;
}
void WII5Sh3dConsole::setCallbackLevel(uint8_t l) {
	callback_level = l;
}
uint8_t WII5Sh3dConsole::getCallbackLevel() {
	return callback_level;
}

// Add a stream - currently very limited (0 NEITHER, 1 INPUT, 2 OUTPUT, 3 BOTH)
bool WII5Sh3dConsole::add(Stream *s, uint8_t direction) {
	for(byte i = 0; i < CONSOLE_STREAMS; i++) {
		if (streams[i] == s) {
			return true;
		}
		else if (! streams[i]) {
			streams[i] = s;
			streamsDirection[i] = direction;
			streamCount++;
			return true;
		}
	}
	// Internal Error !
	return false;
}

bool WII5Sh3dConsole::remove(Stream *s) {
	for(byte i = 0; i < CONSOLE_STREAMS; i++) {
		if (streams[i] == s) {
			streams[i] = NULL;
			streamsDirection[i] = 0;
			streamCount--;
			return true;
		}
	}
	return false;
}

void WII5Sh3dConsole::setLevel(byte l) {
	level = l;
}

// Flush sending....no longer doing inputs
void WII5Sh3dConsole::flush() {
	for(byte i = 0; i < CONSOLE_STREAMS; i++) {
		if (streams[i]) {
			streams[i]->flush();
		}
	}
}

void WII5Sh3dConsole::printNewLine() {
	printf(F("\r\n"));
}

bool WII5Sh3dConsole::enableBinary() {
	binEnable = true;
}
bool WII5Sh3dConsole::disableBinary() {
	binEnable = false;
}
void WII5Sh3dConsole::setBinaryBuffer(char *buf, uint16_t len) {
	binBuffer = buf;
	binMax = len;
	binEnable = true;
}

// Pretty Print Binary
#define PERLINE 10
void WII5Sh3dConsole::printBinary(char* buf, uint16_t len) {

  printf(F("LEN = %d\r\n"), len);
  uint16_t i = 0;
  while (i < len) {

    for (uint16_t x = 0; x < PERLINE; x++) {
      if ( (i+x) < len) {
        printf(F("0x%.2X "), (uint8_t)buf[i+x]);
      }
      else {
        printf(F("     "), (int16_t)buf[i+x]);
      }
    }


    printf(F("  |"));
    for (uint16_t x = 0; x < PERLINE; x++) {
      if (
        ((i+x) < len)
        && ((uint8_t)buf[i+x] >= 32)
        && ((uint8_t)buf[i+x] < 127)
      ) {
        printBuffer[x] = buf[i+x];
      }
      else {
        printBuffer[x] = ' ';
      }

    }
    printBuffer[PERLINE] = '\0';
    print(printBuffer);
    printf(F("|\r\n"));

    i = i + PERLINE;
  }

  printNewLine();
}

void WII5Sh3dConsole::printBits(void* buf, uint16_t len) {
  for (uint16_t x = 0; x < len; x++) {
		void* n = buf + (x/8);
		printf(bitRead(*(uint8_t*)n, x % 8) ? "1" : "0");
	}
	printNewLine();
	SerialConsole.print(F("BIN:  "));
	SerialConsole.print(*(uint16_t*)buf, BIN);
	SerialConsole.println();
}

void WII5Sh3dConsole::setEcho(bool in) {
	echo = in;
}
bool WII5Sh3dConsole::getEcho() {
	return echo;
}

WII5Sh3dConsole console;
