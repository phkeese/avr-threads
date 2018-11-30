#include "threads.hpp"

void tdelay(uint32_t timeout) {
	uint32_t now = millis();
	uint32_t then = now + timeout;
	while (millis() < then) {
		Threads::yield();
	}
}

THREAD t1(void) {
	Serial.println("Thread1");
	Threads::yield();
	while (1) {
		
	}
}

void setup() {
	Serial.begin(9600);
	Serial.println("Before init()");
	Threads::init(100);
	Serial.println("Before yield()");
	Threads::yield();
	Serial.println("After yield()");
	Threads::createThread(t1);
	Serial.println("Next stackbase: " + String((uint16_t)Threads::currentThread->next->stackbase,HEX));
	Serial.println("Next stackptr: " + String((uint16_t)Threads::currentThread->next->stackptr,HEX));
	uint8_t *ptr = (uint8_t*) Threads::currentThread->next->stackptr;
	for (int i = 0; i < 45; i++) {
		Serial.println("SP + " + String(i) + ": " + String(ptr[i],HEX) + "(" + String((char) ptr[i]) + ")");
	}
	delay(100);
	Threads::yield();
}

void loop() {
	Serial.println("Loop");
	// tdelay(100);
}