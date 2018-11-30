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
		if (i >= 5) {
			Serial.println("Worker done!");
			workerDone = true;
			return;
		}
		tdelay(500);
	}
}

Threads::PID workerPID;

void setup() {
	Serial.begin(9600);
	Threads::init(128);
	Threads::createThread(t1);
	workerPID = Threads::createThread(worker);
	
	Threads::Thread *thread = Threads::currentThread;
	Serial.println("[" + String(thread->pid) + "]:0x" + String((uint16_t) thread,HEX));
	while (thread->next != Threads::currentThread) {
		thread = thread->next;
		Serial.println("[" + String(thread->pid) + "]:0x" + String((uint16_t) thread,HEX));
	}
}

int count = 0;
void loop() {
	if (Serial.available()) {
		String echo = Serial.readString();
		Serial.println("Echo: " + echo);
	}
	if (workerDone) {
		Threads::destroyThread(workerPID);
		Serial.println("Destroyed worker thread!");
		workerDone = false;
	}
	Serial.println("Loop!");
	tdelay(500);
}