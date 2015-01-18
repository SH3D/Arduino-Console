#include <Console.h>

Console console();

void setup() {
	Serial.begin(115200);
	console.add(Serial);

	console.printf(F("Hello world\r\n");
}

void loop() {
	console.loop();
	if (console.available()) {
		console.printf(F("CONSOLE: Got command = %c\r\n"), console.getCommand());
	}
}
