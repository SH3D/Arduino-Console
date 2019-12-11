#ifndef WII5Sh3dConsole_h
#define WII5Sh3dConsole_h


#include <Arduino.h>
#include <TimeLib.h>
#include <elapsedMillis.h>
#include <WII5Data.h>

// TODO Default serial port?
#define SerialConsole Serial

// Enable debug flush, if you want regular flushing for crashes etc
#define CONSOLE_DEBUG
// #define CONSOLE_DEBUG_FLUSH

#define CONSOLE_STREAMS 2	// TODO - Can we allocate more if needed?

// Enable to allow Strings in the print lines instead of F() and ""
// #define CONSOLE_USE_STRINGS

// Default levels
#define LOG_NONE -1
#define LOG_FATAL 0
#define LOG_ERROR 1
#define LOG_WARN 2
#define LOG_INFO 3
#define LOG_DEBUG 4
#define LOG_ALL 5

#define CONSOLE_DIRECTION_NONE 0
#define CONSOLE_DIRECTION_INPUT 1
#define CONSOLE_DIRECTION_OUTPUT 2
#define CONSOLE_DIRECTION_BOTH 3

// Default sizex - overloaded in begin
#if defined(__AVR_ATmega644P__) || defined(__AVR_ATmega1284P__)
#define CONSOLE_BUFFER_SIZE 100
#define CONSOLE_COMMAND_SIZE 25
#define VSNPRINTF vsnprintf_P
#elif defined(CORE_TEENSY)
#define CONSOLE_BUFFER_SIZE 200
#define CONSOLE_COMMAND_SIZE 75
#define VSNPRINTF vsnprintf
#else
#define CONSOLE_BUFFER_SIZE 100
#define CONSOLE_COMMAND_SIZE 25
#define VSNPRINTF vsnprintf_P
#endif


#define CONSOLE_TYPE_NMEA_ID 1
#define CONSOLE_TYPE_NMEA_C0 '$'
#define CONSOLE_TYPE_NMEA_NAME "NMEA"

#define CONSOLE_TYPE_CSV_ID 2
#define CONSOLE_TYPE_CSV_C0 '@'
#define CONSOLE_TYPE_CSV_NAME "CSV"
#define CONSOLE_TYPE_CSV_LEN 10

#define CONSOLE_TYPE_BIN_ID 3
#define CONSOLE_TYPE_BIN_C0 '!'
#define CONSOLE_TYPE_BIN_NAME "Binary"
#define CONSOLE_TYPE_BINDATA_ID 4
#define CONSOLE_TYPE_BINDATA_NAME "BinaryData"

#define CONSOLE_TYPE_IGNORE_ID 97
#define CONSOLE_TYPE_DESTROY_ID 98

#define CONSOLE_TYPE_OTHER_ID 99
#define CONSOLE_TYPE_OTHER_C0 ''
#define CONSOLE_TYPE_OTHER_NAME "Standard"

// NOTE: Make sure all of these are wrapped in F() for prints
#define CONSOLE_PRE_INFO "# INFO: "
#define CONSOLE_PRE_WARN "# WARN: "
#define CONSOLE_PRE_ERROR "# ERROR: "
#define CONSOLE_PRE_DEBUG "# DEBUG: "
// Consider better name
#define CONSOLE_PRE_FATAL "# IMPORTANT/NOTSTORED: "
#define CONSOLE_POST_TIME " - "

typedef void (*CallbackFunction) (uint8_t l, char *buf);

// 10 seconds. Really 1 is enough, but slow type debug @ commands maybe
#define CONSOLE_TIMEOUT 30000

class WII5Sh3dConsole {
	public:
		// Begin - create the buffers and allocate etc
		void begin(uint16_t bufSize = CONSOLE_BUFFER_SIZE, uint16_t cmdSize = CONSOLE_COMMAND_SIZE);
		void begin(char* buf, uint16_t bufSize, char* cmdBuf, uint16_t cmdSize);

		// Output streams
		bool add(Stream *s, uint8_t direction = CONSOLE_DIRECTION_BOTH);			// Add normal console, which will process NMEA in
		bool remove (Stream *s);

		// Binary data done in console - must be an external buffer
		bool enableBinary();
		bool disableBinary();	// Stop, Disable, Remove ?
		void setBinaryBuffer(char *buf, uint16_t len);

		// Storage system
		// bool addStorage(Stream *s);
		// bool removeStorage(Stream *s);

		void clearCommand();

		void clearCallback();
		void setCallbackFunction(CallbackFunction f);
		void setCallbackStorage();
		void setCallbackLevel(uint8_t l);
		uint8_t getCallbackLevel();
		void doCallback(uint8_t l);

		void setLevel (uint8_t in);
		int8_t getLevel() { return level; }

		void loop();				// Keep strings up to date
		void flush();
		bool available();			// Report available data (lost on next loop)

		byte getCommand() {return cmd;}
		uint32_t getVal() {return val;}
		char* getBuffer() { return cmdBuffer; }
		void processCmd(char in);

		void processCsv();
		uint8_t getCsvCount() {return csv_count;}
		uint32_t getCsvVal(uint8_t n) {return csv_uint32[n];}
		char* getCsvBuffer(uint8_t n) { return csv_str[n]; }

		// NMEA?

		// Do we show the date/time for logging/errors
		void setDateTime() {displayDateTime = true;}
		void clearDateTime() {displayDateTime = false;}
		void printDateTime(uint32_t t = 0);

		void print(char *out);

		// STREAM printf !
		void sprintf(Stream *s, const __FlashStringHelper *out, ... );

		void printf(char* out, ...);
		void printf(const __FlashStringHelper *out, ... );
		#ifdef CONSOLE_USE_STRINGS
		void printf(String& out, ...);
		#endif

		void printPrefix(uint8_t level);

		void log(uint8_t l, char* out, ...);
		void log(uint8_t l, const __FlashStringHelper *out, ... );
		#ifdef CONSOLE_USE_STRINGS
		void log(uint8_t l, String& out, ...);
		#endif

		// Special handlers
		// void logReduceNumStr(uint8_t l, uint16 num, const __FlashStringHelper *out, ... );
		// void logReduceNum(uint8_t l, uint16 num);

		void safeLog(uint8_t l, char* out, ...);
		void safeLog(uint8_t l, const __FlashStringHelper *out, ... );
		#ifdef CONSOLE_USE_STRINGS
		void safeLog(uint8_t l, String& out, ...);
		#endif

		void nmeaSend(const __FlashStringHelper *out, ... );
		void nmeaCRCSend();

		void printNewLine();

		char *printBuffer; // [CONSOLE_BUFFER_SIZE];
		uint16_t bufferMax;

		bool availableBin() { return binAvailable; }
		char* getBinBuffer() { return binBuffer; }
		uint16_t getBinLength() { return binReceiveCount; }
		void printBinary(char* buf, uint16_t len);
		void printBits(void* buf, uint16_t len);

		WII5Sh3dConsole() {
			level = LOG_ALL;
			callback_level = LOG_NONE;
		}

		void setEcho(bool in);
		bool getEcho();

	protected:

		bool echo;

		void internalErrorNotEnoughMemory();
		void internalErrorNoBufferStream();
		bool check();

		// Timeouts for new lines for safety
		elapsedMillis waitInput;

		CallbackFunction callback;
		int8_t callback_level;
		bool callback_default;

		// Maximum number of streams?
		Stream* streams[CONSOLE_STREAMS];
		uint8_t streamsDirection[CONSOLE_STREAMS];
		uint8_t streamCount = 0;

		// Level of debugging ?
		int8_t level;
		bool displayDateTime;

		// XXX Storage?
		// 	func callback for storage ?

		// Command data
		uint8_t cmdType;
		uint8_t cmdCount;
		byte cmd;
		uint32_t val;

		// Big buffers - only created at start
		// Noite - moved public temporary - char *printBuffer; // [CONSOLE_BUFFER_SIZE];
		char *cmdBuffer; // [CONSOLE_COMMAND_SIZE];

		char *csv_str[CONSOLE_TYPE_CSV_LEN];
		uint32_t csv_uint32[CONSOLE_TYPE_CSV_LEN];
		uint8_t csv_count;	// How many CSV fields do we have so far
		uint8_t csv_last;		// What location in the buffer is it

		// Binary Data external buffers, counts and status
		bool binAvailable;
		char *binBuffer;
		uint16_t binMax;
		bool binEnable;
		uint16_t binReceiveCount;
		uint16_t binReceiveExpect;
		uint16_t binReceiveCRC;

		// Actual allocated memory
		// Note - moved public temporary - uint16_t bufferMax;
		uint16_t consoleMax;

		uint16_t i;
};

// XXX Better name?
extern WII5Sh3dConsole console;

#endif
