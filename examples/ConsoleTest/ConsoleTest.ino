#include <Console.h>
#include <Time.h>

void consolePrintf(const __FlashStringHelper *out, ... );
void consolePrint(const __FlashStringHelper* data, ...);

void setup() {
	delay(2000);

	Serial.begin(115200);
	console.add(&Serial);	// Only adds one, ignores others
	console.add(&Serial);
	console.add(&Serial);

	// XXX if Serial1 add that
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

		console.printf("STRING Level = %d\r\n", console.getLevel());
		console.printf(F("PROGMEM Level = %d\r\n"), console.getLevel());

		console.printf(F("Using PRINTF millis=%d\r\n"), millis());
		console.error(F("Using ERROR millis=%d\r\n"), millis());
		console.warn(F("Using WARN millis=%d\r\n"), millis());
		console.info(F("Using INFO millis=%d\r\n"), millis());
	}
}

// Convert FLASH String to String

void consolePrintf(const __FlashStringHelper *out, ... ){
	char printBuffer[100];
	va_list argptr;
	va_start(argptr, out);
	vsnprintf_P(printBuffer, 100, (const char *)out, argptr);
	va_end(argptr);

	Serial.print("BUFFER=");
	Serial.print(printBuffer);
	Serial.println();

	Serial.print("OUT=");
	Serial.print((const char *)out);
	Serial.println();
}


// Convert FLASH String to String
void consolePrint(const __FlashStringHelper *out, ... ){
	char printBuffer[100];
	va_list argptr;
	va_start(argptr, out);
	vsnprintf(printBuffer, 100, (const char *)out, argptr);
	va_end(argptr);

	Serial.print("BUFFER2=");
	Serial.print(printBuffer);
	Serial.println();
}


