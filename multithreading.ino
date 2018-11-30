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

volatile bool workerDone = false;
THREAD worker(void) {
	int i = 0;
	while (1) {
		Serial.println("Working!");
		i++;
		workerDone = i >= 10;
		tdelay(1000);
	}
}

Threads::PID workerPID;

void setup() {
	Serial.begin(9600);
	Threads::init(128);
	Threads::createThread(t1);
	workerPID = Threads::createThread(worker);
}

int count = 0;
void loop() {
	if (Serial.available()) {
		String echo = Serial.readString();
		Serial.println("Echo: " + echo);
	}
	if (workerDone) {
		Serial.println("Worker done:" + String(workerDone));
		Threads::destroyThread(workerPID);
		workerDone = false;
	}
	tdelay(100);
}