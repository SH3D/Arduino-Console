#ifndef sh3d_console_h
#define sh3d_console_h
#include <Arduino.h>

#define CONSOLE_STREAMS 5
#define CONSOLE_LEVEL_ALL 0
#define CONSOLE_LEVEL_ERROR 1
#define CONSOLE_LEVEL_WARN 2
#define CONSOLE_LEVEL_INFO 3
#define CONSOLE_BUFFER_SIZE 100

class Console {
	public:
		bool addx(Stream *s);
		bool remove (Stream *s);
		void setLevel (uint8_t in);
		byte getLevel() { return level; }

		void loop();				// Keep strings up to date
		bool available();			// Report available data (lost on next loop)
		byte getCommand() {return cmd;}
		uint32_t getVal() {return val;}
		char* getBuffer() { return cmdBuffer; }
		void processCmd(char in);

		void print(char *out);

		void printf(char* out, ...);
		void printf(const __FlashStringHelper *out, ... );
		void printf(String& out, ...);
	
		void info(char* out, ...);
		void info(const __FlashStringHelper *out, ... );
		void info(String& out, ...);
	
		void warn(char* out, ...);
		void warn(const __FlashStringHelper *out, ... );
		void warn(String& out, ...);
	
		void error(char* out, ...);
		void error(const __FlashStringHelper *out, ... );
		void error(String& out, ...);
	
		Console() {}

	protected:
		Stream* streams[5];
		uint8_t level;

		uint8_t cmdCount;
		char cmdBuffer[CONSOLE_BUFFER_SIZE];
		byte cmd;
		uint32_t val;

		char printBuffer[CONSOLE_BUFFER_SIZE];
};

extern Console console;

#endif


