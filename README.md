# Threads v0.6
 A cooperative multithreading library for AVR
 
# Features
- Create up to 65k threads (if your RAM can handle it)
- Easily transfer control to other threads with a single function call
- Destroy threads after use

# Easy to use
To get ready for using threads, simply call `Threads::init(stackSize);`  
To create a new thread, call `Threads::createThread(newThread);`  
To switch to the next thread, call `Threads::yield();`  
To destroy another thread, call `Threads::destroyThread(pid);`  

A simple program might look like this:
```
THREAD workerThread(void) {
   while (1) {
        work();
        Threads::yield();
    }
}

THREAD blinkerThread(void) {
    while (1) {
        blinkLED();
        Threads::yield();
    }
}

int main(void) {
    Threads::init(128);
    Threads::createThread(workerThread);
    Threads::createThread(blinkerThread);
    
    while (1) {
        prepareWork();
        Threads::yield();
        outputWork();
    }
}
```

# Arduino-compatible
Threads does not use any timers or other peripherals, so using it with the Arduino IDE is as simple as including the header. 
An example is provided in `multithreading.ino`  
***Caution: Serial communication should be used sparringly, some functions will block other threads from running***

# Planned features:
- A way of temporarily excluding threads from the queue
- A way of yielding control to a specific thread
