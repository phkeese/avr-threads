#include "threads.hpp"

int on = HIGH;
int pin = 13;

void threadedDelay(int timeout) {
	uint32_t now = millis();
	uint32_t then = now + timeout;
	while (millis() < then) {
		Threads::yield();
	}
}

THREAD thread1(void) {
	threadedDelay(500);
	while (1) {
		digitalWrite(pin,on);
		on = !on;
		threadedDelay(500);
	}
}

THREAD thread2(void) {
	while (1) {
		Serial.println("Thread2 every 3 Seconds");		
		threadedDelay(3000);
	}
}

THREAD threadNever(void) {
	Threads::yield();
}

void setup() {
	// put your setup code here, to run once:
	Serial.begin(2000000);
	Serial.println("Multithreading on the Arduino!");
	pinMode(pin,OUTPUT);
	Serial.println("Thread0: Prints every 1.2 Seconds");
	Serial.println("Thread1: Blinks every Second");
	Serial.println("Thread2: Prints every 3 Seconds");
		
	Threads::init(128);
	Serial.println("Current SP: " + String(Threads::currentThread->stackptr,HEX));
	Threads::createThread(thread1);
	Serial.println("Next SP: " + String(Threads::currentThread->next->stackptr,HEX));
	Serial.println("Next SB: " + String(Threads::currentThread->next->stackbase,HEX));
	uint8_t *ptr = (uint8_t*) Threads::currentThread->next->stackbase;
	Serial.println("Next entry high: " + String(ptr[Threads::settings.stackSize - 2],HEX));
	Serial.println("Next entry: low: " + String(ptr[Threads::settings.stackSize - 1],HEX));
	
	Threads::createThread(thread2);
}

void loop() {
	Serial.println("Thread0 every 1.2 Seconds");
	threadedDelay(1200);
}
