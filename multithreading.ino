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
		Serial.println("T1 Loop");
		tdelay(100);
	}
}

void setup() {
	Serial.begin(9600);
	Threads::init(128);
	Threads::createThread(t1);
	
	Threads::Thread *thread = Threads::currentThread;
	uint16_t highestPID = 0;
	Serial.println("Thread" + String(thread->pid) + ": 0x" + String((uint16_t) thread,HEX));
	while (thread->next->pid > highestPID) {
		thread = thread->next;
		Serial.println("Thread" + String(thread->pid) + ": 0x" + String((uint16_t) thread,HEX));
	}
	
	// Serial.println("Next stackbase: " + String((uint16_t)Threads::currentThread->next->stackbase,HEX));
	// Serial.println("Next stackptr: " + String((uint16_t)Threads::currentThread->next->stackptr,HEX));
	// uint8_t *ptr = (uint8_t*) Threads::currentThread->next->stackptr;
	// for (int i = 0; i < 45; i++) {
		// Serial.println("SP + " + String(i) + ": " + String(ptr[i],HEX) + "(" + String((char) ptr[i]) + ")");
	// }
	// delay(100);
	Serial.println("Last Thread:" + String((uint16_t)Threads::getLastThread(),HEX));
	Serial.println("Current:" + String((uint16_t)Threads::currentThread,HEX));
	Serial.println("Next:" + String((uint16_t)Threads::currentThread->next,HEX));
	//Threads::yield();
}

void loop() {
	Serial.println("Loop");
	// Threads::yield();
	tdelay(100);
}