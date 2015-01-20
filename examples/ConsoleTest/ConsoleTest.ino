#include <Console.h>
#include <Time.h>

void setup() {
	Serial.begin(115200);
	console.add(&Serial);	// Only adds one, ignores others
	console.add(&Serial);
	console.add(&Serial);

	#ifdef Serial1
	Serial1.begin(57600);
	console.add(&Serial1);
	#endif

	// TODO write an example logging to an SD card

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
		if (console.getCommand() == 'a')
			console.setLevel(CONSOLE_LEVEL_ALL);	// Show all
		else if (console.getCommand() == 'e')
			console.setLevel(CONSOLE_LEVEL_ERROR);	// Show all ?
		else if (console.getCommand() == 'w')
			console.setLevel(CONSOLE_LEVEL_WARN);	// Warnings and 
		else if (console.getCommand() == 'i')
			console.setLevel(CONSOLE_LEVEL_INFO);	// Show all ?
		else if (console.getCommand() == 'd')
			console.setDateTime();
		else if (console.getCommand() == 'c')
			console.clearDateTime();

		console.printf(F("Level = %d\r\n"), console.getLevel());

		console.printf(F("Using PRINTF millis=%d\r\n"), millis());
		console.error(F("Using ERROR millis=%d\r\n"), millis());
		console.warn(F("Using WARN millis=%d\r\n"), millis());
		console.info(F("Using INFO millis=%d\r\n"), millis());
	}
}
