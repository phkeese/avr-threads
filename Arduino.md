# Using Threads with Arduino 
Tipps and tricks for using Threads with the Arduino library.

### 1. Serial Communication
You *can* use the methods provided by the `Serial` class, however there are some limitations.
#### 1.1. Reading from Serial
Normally, you would do something like `String mystring = Serial.readString()` to read input from the user. In my experience,  doing so anywhere else than within the `loop` function will lead to errors. The same goes for using `Serial.available()` anywhere else. I suspect it has to do with the call to `serialEventRun` after the call to `loop`, but I am not certain.
If you have to read user input, do it in `loop`.
#### 1.2. Writing to Serial
As with `Serial.readString()`, `Serial.println()` also behaves differently out of `loop`. You can write constant strings, like `Serial.println("Hello, world!");`, but integrating numbers into the message like `Serial.println("5!=" + String(120));` will most likely fail. These functions should work as expected within `loop`.

### 2. Delays
The standard `delay` function of the Arduino library blocks the CPU until the timeout is reached. A way of having delays in threads could look like this:
```
void threadedDelay(uint32_t timeout) {
    uint32_t now = millis();
    uint32_t then = now + timeout;
    while (millis() < then) {
        Threads::yield();
    }
}
```
This function will keep switching to the next thread, until the timeout is reached.
