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
	Threads::createThread(thread1);
	Threads::createThread(thread2);
}

void loop() {
	Serial.println("Thread0 every 1.2 Seconds");
	threadedDelay(1200);
}
