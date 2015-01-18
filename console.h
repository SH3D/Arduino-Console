#ifndef sh3d_console_h
#define sh3d_console_h
#include <Arduino.h>

#define CONSOLE_LEVEL_ALL 0
#define CONSOLE_LEVEL_ERROR 1
#define CONSOLE_LEVEL_WARN 2
#define CONSOLE_LEVEL_INFO 3

class Console {
	public:
		add(Stream *s);
		remove (Stream *s);
		level (uint8_t in);

		void loop();				// Keep strings up to date
		bool available();			// Report available data (lost on next loop)
		byte getCommand();
		uint32_t getVal();
		char* getBuffer() { return cmdBuffer; }

		print(char *out);

		printf(char* out, ...);
		printf(const __FlashStringHelper *out, ... );
		printf(String& out, ...);
	
		info(char* out, ...);
		info(const __FlashStringHelper *out, ... );
		info(String& out, ...);
	
		warn(char* out, ...);
		warn(const __FlashStringHelper *out, ... );
		warn(String& out, ...);
	
		error(char* out, ...);
		error(const __FlashStringHelper *out, ... );
		error(String& out, ...);
	
		Console();

	protected:
		Stream* streams[5];
		uint8_t l;

		uint8_t cmdCount;
		char cmdBuffer[CONSOLE_BUFFER_SIZE];
		byte cmd;
		uint32_t val;
};

#endif

