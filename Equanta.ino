
#include <Bounce2.h>
#include <math.h>

// Pins for the encoder
const int encoderPinA = 2;
const int encoderPinB = 4;

const int buttonPin = 7;
const int ledPin = 3;
const int relayPin = 6;
bool debug = false;

// Variables to track the encoder position
volatile int encoderPos = 0;
volatile int lastEncoded = 0;

const int batteryFullEncoder = 200;

// Instantiate Bounce object for debouncing the button
Bounce buttonDebouncer = Bounce();

void setup() {
  if (debug)
  {
    Serial.begin(115200);
    Serial.println("Encoder Test");
  }

  // Set up encoder pins
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);
  // Button
  pinMode(buttonPin, INPUT_PULLUP);
  buttonDebouncer.attach(buttonPin);
  buttonDebouncer.interval(50); // Set debounce interval to 50 milliseconds

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);  
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  
}
int loop_count = 1;

void loop() {
  int MSB = digitalRead(encoderPinA); // MSB = most significant bit
  int LSB = digitalRead(encoderPinB); // LSB = least significant bit

  int encoded = (MSB << 1) | LSB; // Converting the 2 pin value to single number
  int sum = (lastEncoded << 2) | encoded; // Adding it to the previous encoded value

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderPos++;
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderPos++;

  lastEncoded = encoded;
  
  if (encoderPos >= batteryFullEncoder)
  {
//    int sign = abs(encoderPos)/encoderPos;
//    encoderPos = batteryFullEncoder * sign;
    encoderPos = 0;
    //glow light and wait for button press
    glowLightAndWaitForButton();
    digitalWrite(relayPin, HIGH);
    delay(23000);
  }
  digitalWrite(relayPin, LOW);
  if (debug)
  {
    // Print the encoder position
    Serial.print("Encoder position: \t");
    Serial.print(encoderPos);
  
    Serial.print("\t\tMSB: \t");
    Serial.print(digitalRead(encoderPinA));
    
    Serial.print("\t\tLSB: \t");
    Serial.println(digitalRead(encoderPinB));
  }
  
  if (loop_count % 25 == 0)
  {
    if (abs(encoderPos) > 0)
      encoderPos -= encoderPos/abs(encoderPos);
    loop_count=1;
  }
  loop_count++;

  updateLEDBrightness(encoderPos);
  
  delay(1); // Update every 100 ms
}

void updateLEDBrightness(int encoderPos) {
  int absPos = abs(encoderPos); // Get the absolute value of the encoder position
  int brightness = map(absPos, 0, batteryFullEncoder, 0, 255); // Map the position to a brightness value
  analogWrite(ledPin, 255 - brightness); // Invert the brightness for the LED
}
void glowLightAndWaitForButton() {
  // Pulse the LED
  while (true) {
    // Calculate the brightness for the pulsing effect
    unsigned long currentTime = millis();
    float angle = 2.0 * PI * (currentTime % 1000) / 1000.0; // 1 second period
    int brightness = 127 + 127 * sin(angle); // Sine wave

    // Set the LED brightness (inverted)
    analogWrite(ledPin, 255 - brightness);

    // Check if the button is pressed
    buttonDebouncer.update();
    if (buttonDebouncer.fell()) {
      // Turn off the LED and break the loop when button is pressed
      digitalWrite(ledPin, HIGH);
      break;
    }
  }
}
