// https://howtomechatronics.com/tutorials/arduino/arduino-brushless-motor-control-tutorial-esc-bldc/

#include <Servo.h>

Servo ESC;     // create servo object to control the ESC

#define LED 2
#define BUTTON 3
#define MOTOR A0


void setup() {

    pinMode(LED, OUTPUT);
    pinMode(BUTTON, INPUT_PULLUP); // on means off, off means on; this is to make the button state much clearer to the arduino
    pinMode(MOTOR, OUTPUT);

    // Attach the ESC to MOTOR pin
    ESC.attach(MOTOR,1100,1900); // (pin, min pulse width, max pulse width in microseconds) 
}

calculateSpeed() {
    
    // increase over time (blocking)
};

void loop() {
    digitalWrite(LED, LOW);
    while (digitalRead(BUTTON)) { // wait until button is pressed
        delay(100);
    }

    {
        // blink every 200ms for 5 seconds for priming (to make sure it doesnt move and accidentally trigger)
        uint16_t primingPeriod = 5000;
        uint16_t blinkInterval = 200;
        for(uint16_t i; i < (primingPeriod / blinkInterval); i++) { // todo put variable outside to avoid calculating every time
            digitalWrite(LED, (!digitalRead(LED))); // set to opposite, meaning it instantly turns off until next call which turns it back on (the opposite state)
            delay(blinkInterval);        
        }
    }

    while (!digitalRead(BUTTON)) { // allow to be cancelled with button press (hold? because blocking delays)
        uint8_t calculatedValue;  // value from the analog pin

        // calculate and constrain to 75% power
        calculatedValue = map(constrain(calculateSpeed(), 0, 75), 0, 100, 0, 180);   // scale it to use it with the servo library (value between 0 and 180)
        ESC.write(calculatedValue);    // Send the signal to the ESC
    }
}