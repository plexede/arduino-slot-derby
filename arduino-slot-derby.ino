// https://howtomechatronics.com/tutorials/arduino/arduino-brushless-motor-control-tutorial-esc-bldc/

#include <Servo.h>

Servo ESC;     // create servo object to control the ESC

#define LED 2
#define BUTTON 3
#define MOTOR A0


void setup() {
    // Attach the ESC on pin 9
    ESC.attach(MOTOR,1100,1900); // (pin, min pulse width, max pulse width in microseconds) 
}

calculateSpeed() {
    
    // increase over time (blocking)
};

void loop() {
    digitalWrite(LED, LOW);
    while (digitalRead(BUTTON)) {
        delay(100);
    }
    digitalWrite(LED, HIGH);
    while (digitalRead(BUTTON)) {
        uint8_t calculatedValue;  // value from the analog pin
        // fix this up
        calculatedValue = map(constrain(calculateSpeed(), 0, 75), 0, 100, 0, 180);   // scale it to use it with the servo library (value between 0 and 180)
        ESC.write(calculatedValue);    // Send the signal to the ESC
    }
}