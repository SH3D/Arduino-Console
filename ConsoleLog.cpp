/*

	Console code - Scott Penrose <scottp@dd.com.au>

*/
#include <Arduino.h>
#include "ConsolePrint.h"

void WII5Sh3dConsole::setLevel(byte l) {
	level = l;
}

// TODO Move to Sh3dNodeErrors
void Console::printPrefix(uint8_t level) {
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
void Console::log(uint8_t l, char* out, ...) {
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
void Console::log(uint8_t l, const __FlashStringHelper *out, ... ){
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
void Console::safeLog(uint8_t l, char* out, ...) {
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

void Console::safeLog(uint8_t l, const __FlashStringHelper *out, ... ){
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
