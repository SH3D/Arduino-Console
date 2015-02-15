#ifndef sh3d_console_h
#define sh3d_console_h
#include <Arduino.h>
#include <Time.h>

#define CONSOLE_STREAMS 5
#define CONSOLE_LEVEL_ERROR 0
#define CONSOLE_LEVEL_WARN 1
#define CONSOLE_LEVEL_INFO 2
#define CONSOLE_LEVEL_ALL 3
#if defined(__AVR_ATmega644P__) || defined(__AVR_ATmega1284P__)
#define CONSOLE_BUFFER_SIZE 150
#define CONSOLE_COMMAND_SIZE 50
#else
#define CONSOLE_BUFFER_SIZE 140
#define CONSOLE_COMMAND_SIZE 25
#endif

// TODO - consider making these configurable...
#define CONSOLE_PRE_INFO "INFO: "
#define CONSOLE_PRE_WARN "WARN: "
#define CONSOLE_PRE_ERROR "ERROR: "
#define CONSOLE_POST_TIME " - "

class Console {
	public:
		bool add(Stream *s);
		bool remove (Stream *s);
		void setLevel (uint8_t in);
		byte getLevel() { return level; }

		void loop();				// Keep strings up to date
		bool available();			// Report available data (lost on next loop)
		byte getCommand() {return cmd;}
		uint32_t getVal() {return val;}
		char* getBuffer() { return cmdBuffer; }
		void processCmd(char in);

		void setDateTime() {displayDateTime = true;}
		void clearDateTime() {displayDateTime = false;}
		void printDateTime(uint32_t t = 0);

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
	
		Console() {level = CONSOLE_LEVEL_ALL;}

	protected:
		Stream* streams[5];
		uint8_t level;
		bool displayDateTime;

		uint8_t cmdCount;
		char cmdBuffer[CONSOLE_COMMAND_SIZE];
		byte cmd;
		uint32_t val;

		char printBuffer[CONSOLE_BUFFER_SIZE];
};

extern Console console;

#endif
