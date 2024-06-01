#include "HX711.h"
#include <Servo.h>
#include <FastLED.h>

// Define the number of LEDs
#define NUM_LEDS 300

// Define the data pin
#define DATA_PIN 8

// Create a servo object
Servo door_1;
const int offset = 80;
const int Door_CAN = 50;
const int Door_Trash = 100;
const int rest = 80;
const int CAN_ang = Door_CAN + offset;
const int Trash_ang = Door_Trash + offset;


// HX711 circuit wiring
const int servoPin = 9;
int pos = 0;
float reading = 0;
const int LOADCELL_DOUT_PIN = 3;
const int LOADCELL_SCK_PIN = 2;

// Create a CRGB array to hold the LED data
CRGB leds[NUM_LEDS];

// Create an instance of the HX711 class
HX711 scale;
float calibration_factor = -950; // Zero factor: -111683

void setup() {
  Serial.begin(9600);
  
  // Initialize the FastLED library
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  
  // Set the brightness of the LEDs
  FastLED.setBrightness(10); // Adjust the brightness as needed
  
  pinMode(0, INPUT); // Induction sensor input
  pinMode(1, INPUT); //
  
  door_1.attach(servoPin);
  door_1.write(rest);
  delay(1000);
  
  // Initialize the HX711
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale();
  scale.tare();
  delay(1000);
}

void loop() {
  scale.set_scale(calibration_factor);
  
  // Check if the scale is ready to read
  delay(1000);
  
  if (scale.is_ready()) {
    
    float reading_1 = scale.get_units(); // Get the raw data from the HX711
    delay(250); // Wait for 250 milliseconds
    float reading_2 = scale.get_units();
    float reading = ((reading_1 +reading_2)/2);
    Serial.println(reading);
    
    // Read the induction sensor value from A0
    int inductionSensorValueA0 = analogRead(A0);
    bool isInductionHighA0 = inductionSensorValueA0 > 900; // Adjust the threshold as necessary
    
    // Read the induction sensor value from A1
    int inductionSensorValueA1 = analogRead(A1);
    bool isInductionHighA1 = inductionSensorValueA1 > 900; // Adjust the threshold as necessary
    
    Serial.println(isInductionHighA0);
    Serial.println(isInductionHighA1);
    
    if (reading > 10 && (isInductionHighA0 || isInductionHighA1) && reading < 27) { // Add a weight range
      Serial.println("CAN");
      door_1.write(CAN_ang);
      pos = CAN_ang;
      for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Green;
      }
      FastLED.show(); // Update the LEDs
      delay(525);
          float reading = scale.get_units(); // Get the raw data from the HX711
          while (pos != rest && reading < 1070) { 
            float reading = scale.get_units(); // Get the raw data from the HX711
            pos -=10;
            door_1.write(pos);              // tell servo to go to position in variable 'pos'                     // waits 15 ms for the servo to reach the position
                for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::White;
    }
    FastLED.show(); // Update the LEDs
          }
        
    } else if (reading > 4) {
      Serial.println("TRASH");
      pos = Trash_ang;
      door_1.write(pos);
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Red;
      }
      FastLED.show(); // Update the LEDs
      delay(700);
  
          float reading = scale.get_units(); // Get the raw data from the HX711
          Serial.println(reading);
          while (pos != rest && reading < 1070) { 
            float reading = scale.get_units(); // Get the raw data from the HX711
            pos -=10;
            door_1.write(pos);              // tell servo to go to position in variable 'pos'
                     // waits 15 ms for the servo to reach the position
               for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::White;
    }
    FastLED.show(); // Update the LEDs
          }
    }
    
    // Turn off all LEDs

    
//    FastLED.clear(); // Clear all pixel data
//    FastLED.show();
    
  } else {
    Serial.println("HX711 not ready");
  }
}
