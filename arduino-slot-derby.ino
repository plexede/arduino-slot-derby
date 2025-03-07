// https://howtomechatronics.com/tutorials/arduino/arduino-brushless-motor-control-tutorial-esc-bldc/

#include <Servo.h>
#include <Arduino_LSM6DS3.h>  // https://docs.arduino.cc/libraries/arduino_lsm6ds3/

Servo ESC;  // create servo object to control the ESC

#define LED 2
#define BUTTON 3
#define MOTOR A0

#define IMU_DEADZONE 10;  // PLAY WITH THIS (the number i put is completely random) "Accelerometer range is set at ±4 g with a resolution of 0.122 mg."


void setup() {
  Serial.begin(115200);

  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);  // on means off, off means on; this is to make the button state much clearer to the arduino
  pinMode(MOTOR, OUTPUT);

  // Attach the ESC to MOTOR pin
  ESC.attach(MOTOR, 1100, 1900);  // (pin, min pulse width, max pulse width in microseconds)

  // initialize IMU
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1)
      ;
  }
}

calculateSpeed(){

  // increase over time (blocking)
};

void loop() {
  digitalWrite(LED, LOW);
  while (digitalRead(BUTTON)) {  // wait until button is pressed
    delay(100);
  }

  // priming period (to make sure it doesnt move and accidentally trigger)
  {
    // blink every 200ms for 5 seconds for priming
    uint16_t primingPeriod = 5000;
    uint16_t blinkInterval = 200;
    for (uint16_t i; i < (primingPeriod / blinkInterval); i++) {  // todo put variable outside to avoid calculating every time
      digitalWrite(LED, (!digitalRead(LED)));                     // set to opposite, meaning it instantly turns off until next call which turns it back on (the opposite state)
      delay(blinkInterval);
    }
  }
  digitalWrite(LED, HIGH);  // ensure its on at the end because im too lazy to do math

  // idle until car falls
  {
    // solve the time in between IMU samples  (see comment on delay call)
    float resolvedSampleRate = (1 / IMU.accelerationSampleRate());
    int toDelay = round(resolvedSampleRate * 1000.0);  // automatically rounds it

    // wait until car falls
    while (1) {  // anticipate break
      float x, y, z;

      if (IMU.accelerationAvailable()) {
        IMU.readAcceleration(x, y, z);              // automatically sets variables to returned values
        if ((x + y + z) > IMU_DEADZONE) { break; }  // exit the loop if tripped
      }

      // delay the time it takes to get the next IMU reading. inherently inneffective towards the latency, while probably saving battery.
      delay(toDelay);
    }
    IMU.end();  // turn off IMU when finished
  }

  while (!digitalRead(BUTTON)) {  // allow to be cancelled with button press (hold? because blocking delays)
    uint8_t calculatedValue;      // value from the analog pin

    // calculate and constrain to 75% power
    calculatedValue = map(constrain(calculateSpeed(), 0, 75), 0, 100, 0, 180);  // scale it to use it with the servo library (value between 0 and 180)
    ESC.write(calculatedValue);                                                 // Send the signal to the ESC
  }
}