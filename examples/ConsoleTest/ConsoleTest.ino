#include "Console.h"

void setup() {
	Serial.begin(115200);
	console.add(&Serial);
	console.add(&Serial);
	console.add(&Serial);
	console.printf(F("Hello world\r\n"));

	console.printf("Standard Characters - %d\r\n", 1);
	console.printf(F("PROGMEM Characters - %d\r\n"), 2);
	String str = "String characters - %d\r\n";
	console.printf(str, 3);
}

void loop() {
	console.loop();
	if (console.available()) {
		console.printf(F("CONSOLE: Got command %c=%d \r\n"), 
			console.getCommand(),
			console.getVal()
		);
	}
}
