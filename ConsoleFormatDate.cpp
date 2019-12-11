/*

	Console code - Scott Penrose <scottp@dd.com.au>

void WII5Sh3dConsole::printDateTime(uint32_t t) {
	if (t == 0) {
		if (!displayDateTime) return;	// Automatic mode
		#ifdef _Time_h
		t = now();
		#endif
	}
	//	uint16_t - date.year,
	//	uint8_t - date.month, date.day,
	//	uint8_t - time.hour, time.minute, time.second
	sprintf_P(printBuffer, PSTR("%04d-%02d-%02dT%02d:%02d:%02d"),
		year(t),
		int(month(t)),
		int(day(t)),
		int(hour(t)),
		int(minute(t)),
		int(second(t))
	);
	print(printBuffer);
}
