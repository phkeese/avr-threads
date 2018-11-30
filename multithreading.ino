#include "threads.hpp"

void tdelay(uint32_t timeout) {
	uint32_t now = millis();
	uint32_t then = now + timeout;
	while (millis() < then) {
		Threads::yield();
	}
}

THREAD t1(void) {
	while (1) {
		Serial.println("T1 Loop");
		tdelay(1000);
	}
}

void setup() {
	Serial.begin(9600);
	Threads::init(128);
	Threads::createThread(t1);
}

void loop() {
	Serial.println("Loop");
	if (serialEventRun) {
		serialEventRun();
	}
	tdelay(100);
}