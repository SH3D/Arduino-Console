/*

	Console code - Scott Penrose <scottp@dd.com.au>

*/

// Pretty Print Binary
#define PERLINE 10
void WII5Sh3dConsole::printBinary(char* buf, uint16_t len) {

  printf(F("LEN = %d\r\n"), len);
  uint16_t i = 0;
  while (i < len) {

    for (uint16_t x = 0; x < PERLINE; x++) {
      if ( (i+x) < len) {
        printf(F("0x%.2X "), (uint8_t)buf[i+x]);
      }
      else {
        printf(F("     "), (int16_t)buf[i+x]);
      }
    }


    printf(F("  |"));
    for (uint16_t x = 0; x < PERLINE; x++) {
      if (
        ((i+x) < len)
        && ((uint8_t)buf[i+x] >= 32)
        && ((uint8_t)buf[i+x] < 127)
      ) {
        printBuffer[x] = buf[i+x];
      }
      else {
        printBuffer[x] = ' ';
      }

    }
    printBuffer[PERLINE] = '\0';
    print(printBuffer);
    printf(F("|\r\n"));

    i = i + PERLINE;
  }

  printNewLine();
}

void WII5Sh3dConsole::printBits(void* buf, uint16_t len) {
  for (uint16_t x = 0; x < len; x++) {
		void* n = buf + (x/8);
		printf(bitRead(*(uint8_t*)n, x % 8) ? "1" : "0");
	}
	printNewLine();
	SerialConsole.print(F("BIN:  "));
	SerialConsole.print(*(uint16_t*)buf, BIN);
	SerialConsole.println();
}
