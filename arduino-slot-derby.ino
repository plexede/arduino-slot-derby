// https://howtomechatronics.com/tutorials/arduino/arduino-brushless-motor-control-tutorial-esc-bldc/

#include <Servo.h>
#include <Arduino_LSM6DS3.h>  // https://docs.arduino.cc/libraries/arduino_lsm6ds3/

Servo ESC;  // create servo object to control the ESC

#define LED 2
#define BUTTON 3
#define MOTOR A0

// ESC pulse width in microseconds
#define MIN_PULSE_WIDTH 1000
#define MAX_PULSE_WIDTH 2000


// blink every 200ms for 5 seconds for priming (to make sure it doesnt move and accidentally trigger)
#define PRIMING_PERIOD 5000
#define BLINK_INTERVAL 200

#define IMU_DEADZONE 0  // PLAY WITH THIS (the number i put is completely random) "Accelerometer range is set at ±4 g with a resolution of 0.122 mg." (whatever that means)


void setup() {
  Serial.begin(115200);

  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);  // on means off, off means on; this is to make the button state much clearer to the arduino
  pinMode(MOTOR, OUTPUT);

  // Attach the ESC to MOTOR pin
  // ESC.writeMicroseconds(1500); // https://forum.arduino.cc/t/help-me-understanding-controlling-brushless-motor/1157800/23
  ESC.attach(MOTOR, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);  // (pin, min pulse width, max pulse width in microseconds)
}

uint8_t calculateSpeed() {
  // increase over time
  uint8_t toReturn = 100;             // try to return 100% speed
  return constrain(toReturn, 0, 75);  // clamp to 75% speed and return as function output.
}

void loop() {
  digitalWrite(LED, LOW);
  // initialize IMU
  while (digitalRead(BUTTON)) {  // wait until button is pressed
    delay(100);
  }
  if (!IMU.begin()) {
    while (1) {
      Serial.println("Failed to initialize IMU!");
    }
  }

  // priming period (to make sure it doesnt move and accidentally trigger)
  // blink every 200ms for 5 seconds for priming
  for (uint16_t i; i < (PRIMING_PERIOD / BLINK_INTERVAL); i++) {  // todo put variable outside to avoid calculating every time
    digitalWrite(LED, (!digitalRead(LED)));                     // set to opposite, meaning it instantly turns off until next call which turns it back on (the opposite state)
    delay(BLINK_INTERVAL);
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

  while (digitalRead(BUTTON)) {  // allow to be cancelled with button press (hold? because blocking delays)
    uint8_t calculatedValue;     // value from the analog pin
    // call speed calculatation function
    calculatedValue = map(calculateSpeed(), 0, 100, 0, 180);  // scale it to use it with the servo library (value between 0 and 180)
    ESC.writeMicroseconds(calculatedValue);                               // Send the signal to the ESC
  }
    ESC.writeMicroseconds(0);
}