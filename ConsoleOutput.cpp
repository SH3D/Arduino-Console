/*

    Console code - Scott Penrose <scottp@dd.com.au>

 */
#include <Arduino.h>
#include "ConsolePrint.h"

// Add a stream - currently very limited (0 NEITHER, 1 INPUT, 2 OUTPUT, 3 BOTH)
bool Console::add(Stream *s, uint8_t direction) {
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

bool Console::remove(Stream *s) {
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

void Console::print(char *out) {
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
