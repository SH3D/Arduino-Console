/*

    Console code - Scott Penrose <scottp@dd.com.au>

 */
#include <Arduino.h>
#include "ConsolePrint.h"

// sprintf - STREAM printf - XXX
void Console::sprintf(Stream *s, const __FlashStringHelper *out, ... ) {
    if (!check()) return;
    va_list argptr;
    va_start(argptr, out);
    VSNPRINTF(printBuffer, bufferMax, (const char *)out, argptr);
    va_end(argptr);
    s->print(printBuffer);
}

// printf support
void Console::printf(char* out, ...) {
    if (!check()) return;
    va_list argptr;
    va_start(argptr, out);
    vsnprintf(printBuffer, bufferMax, out, argptr);
    va_end(argptr);
    print(printBuffer);
}

// Convert FLASH String to String
void Console::printf(const __FlashStringHelper *out, ... ){
    if (!check()) return;
    va_list argptr;
    va_start(argptr, out);
    VSNPRINTF(printBuffer, bufferMax, (const char *)out, argptr);
    va_end(argptr);
    print(printBuffer);
}

void Console::printNewLine() {
    printf(F("\r\n"));
}
