#include <Arduino.h>
#include <Bounce.h>
#include <SparkFunSX1509.h>  // Library added manually because that's the platformio way if it is not in the standard catalog?
                             // Github: https://github.com/sparkfun/SparkFun_SX1509_Arduino_Library
#include <Wire.h>

// Multiplexer chip sx1509 stuff.
const byte SX1509_ADDRESS = 0x3E;

const byte SX1509_BUTTON1LED = 14;     
const byte SX1509_BUTTON1BUTTON = 15;
const byte SX1509_BUTTON2LED = 12;
const byte SX1509_BUTTON2BUTTON = 13;
const byte SX1509_BUTTON3LED = 10;
const byte SX1509_BUTTON3BUTTON = 11;
const byte SX1509_BUTTON4LED = 8;
const byte SX1509_BUTTON4BUTTON = 9;
const byte SX1509_BUTTON5LED = 6;
const byte SX1509_BUTTON5BUTTON = 7;
const byte SX1509_BUTTON6LED = 4;
const byte SX1509_BUTTON6BUTTON = 5;
const byte SX1509_BUTTON7LED = 2;
const byte SX1509_BUTTON7BUTTON = 3;
const byte SX1509_BUTTON8LED = 0;
const byte SX1509_BUTTON8BUTTON = 1;

const byte SX1509_INTERRUPT_PIN = 20;

#define sdaPin 18
#define sclPin 19
SX1509 io;

Bounce button0 = Bounce(23, 20);
Bounce button1 = Bounce(22, 20);  // 10 ms debounce time is appropriate
Bounce button2 = Bounce(21, 20);  // for most mechanical pushbuttons
Bounce button3 = Bounce(20, 20);  // These feel noisy, 20ms it is.
Bounce button4 = Bounce(19, 20);

unsigned long lastAction = 0;
unsigned long currentMillis = 0;

byte buttonPushed = 0;

const byte numberOfButtons = 22;

// State store in 2d array.
const byte numberOfStates = 1;
const byte active = 0;
byte state[numberOfStates][numberOfButtons];


void processButtons() {
 buttonPushed = 1;
}

void multiplexerSetup() {
  // The SX1509 interrupt is active-low.
  pinMode(SX1509_INTERRUPT_PIN, INPUT_PULLUP);

  //Wire.begin(sdaPin, sclPin);
  Wire.begin();

  // Call io.begin(<address>) to initialize the SX1509. If it
  // successfully communicates, it'll return 1.
  if (io.begin(SX1509_ADDRESS) == false) {
    Serial.println("Failed to communicate. Check wiring and address of SX1509.");
    digitalWrite(22, HIGH);  // If we failed to communicate, turn the pin 13 LED on
    while (1) {
      digitalWrite(13, 1);
      delay(200);
      digitalWrite(13, 0);
      delay(200);
    }
  }

  io.debounceTime(20);  // 10ms is default, 20 is a bit more noisy compatible.

  io.pinMode(SX1509_BUTTON1LED, OUTPUT);
  io.pinMode(SX1509_BUTTON1BUTTON, INPUT_PULLUP);
  io.pinMode(SX1509_BUTTON2LED, OUTPUT);
  io.pinMode(SX1509_BUTTON2BUTTON, INPUT_PULLUP);
  io.pinMode(SX1509_BUTTON3LED, OUTPUT);
  io.pinMode(SX1509_BUTTON3BUTTON, INPUT_PULLUP);
  io.pinMode(SX1509_BUTTON4LED, OUTPUT);
  io.pinMode(SX1509_BUTTON4BUTTON, INPUT_PULLUP);
  io.pinMode(SX1509_BUTTON5LED, OUTPUT);
  io.pinMode(SX1509_BUTTON5BUTTON, INPUT_PULLUP);
  io.pinMode(SX1509_BUTTON6LED, OUTPUT);
  io.pinMode(SX1509_BUTTON6BUTTON, INPUT_PULLUP);
  io.pinMode(SX1509_BUTTON7LED, OUTPUT);
  io.pinMode(SX1509_BUTTON7BUTTON, INPUT_PULLUP);
  io.pinMode(SX1509_BUTTON8LED, OUTPUT);
  io.pinMode(SX1509_BUTTON8BUTTON, INPUT_PULLUP);

  io.enableInterrupt(SX1509_BUTTON1BUTTON, FALLING);
  io.enableInterrupt(SX1509_BUTTON2BUTTON, FALLING);
  io.enableInterrupt(SX1509_BUTTON3BUTTON, FALLING);
  io.enableInterrupt(SX1509_BUTTON4BUTTON, FALLING);
  io.enableInterrupt(SX1509_BUTTON5BUTTON, FALLING);
  io.enableInterrupt(SX1509_BUTTON6BUTTON, FALLING);
  io.enableInterrupt(SX1509_BUTTON7BUTTON, FALLING);
  io.enableInterrupt(SX1509_BUTTON8BUTTON, FALLING);

  io.debouncePin(SX1509_BUTTON1BUTTON);  
  io.debouncePin(SX1509_BUTTON2BUTTON);  
  io.debouncePin(SX1509_BUTTON3BUTTON);  
  io.debouncePin(SX1509_BUTTON4BUTTON); 
  io.debouncePin(SX1509_BUTTON5BUTTON);   
  io.debouncePin(SX1509_BUTTON6BUTTON);     
  io.debouncePin(SX1509_BUTTON7BUTTON);   
  io.debouncePin(SX1509_BUTTON8BUTTON);
  
  attachInterrupt(digitalPinToInterrupt(SX1509_INTERRUPT_PIN), processButtons, FALLING);

  // Blink the LED a few times before we start:
  for (int i = 0; i < 2; i++) {
    // Use io.digitalWrite(<pin>, <LOW | HIGH>) to set an
    // SX1509 pin either HIGH or LOW:
    io.digitalWrite(SX1509_BUTTON7LED, HIGH);
    io.digitalWrite(SX1509_BUTTON8LED, HIGH);
    delay(100);
    io.digitalWrite(SX1509_BUTTON7LED, LOW);
    io.digitalWrite(SX1509_BUTTON8LED, LOW);
    delay(100);
    io.digitalWrite(SX1509_BUTTON7LED, HIGH);
    io.digitalWrite(SX1509_BUTTON8LED, HIGH);
    delay(100);
    io.digitalWrite(SX1509_BUTTON7LED, LOW);
    io.digitalWrite(SX1509_BUTTON8LED, LOW);
  }
  //io.pinMode(SX1509_WHITELED1, ANALOG_OUTPUT);
}


void setup() {
  /*pinMode(23, INPUT_PULLUP);
  pinMode(22, INPUT_PULLUP);
  pinMode(21, INPUT_PULLUP);
  pinMode(20, INPUT_PULLUP);
  pinMode(19, INPUT_PULLUP);*/
  pinMode(13, OUTPUT);
  digitalWrite(13, 1);
  delay(200);
  digitalWrite(13, 0);
  delay(200);
  

  Keyboard.begin();
  
  Serial.begin(115200);

  digitalWrite(13, 1);
  delay(200);
  digitalWrite(13, 0);
  delay(200);
  
  
  //while (!Serial);
  multiplexerSetup();
  Serial.println("hoi");
  digitalWrite(13, 1);
  delay(200);
  digitalWrite(13, 0);
  delay(200);
}

void doMultiplexedButtons() {
  // read io.interruptSource() find out which pin generated
  // an interrupt and clear the SX1509's interrupt output.
  unsigned int intStatus = io.interruptSource();
  // For debugging handiness, print the intStatus variable.
  // Each bit in intStatus represents a single SX1509 IO.
  Serial.println("intStatus = " + String(intStatus, BIN));

  if (intStatus & (1 << SX1509_BUTTON1BUTTON)) {
    Serial.println("Button 1 pressed!");
    Keyboard.print(currentMillis/10);
    Keyboard.println(" a");
  }
  if (intStatus & (1 << SX1509_BUTTON2BUTTON)) {
    Serial.println("Button 2 pressed!");
    Keyboard.print(currentMillis/10);
    Keyboard.println(" b");
  }
  if (intStatus & (1 << SX1509_BUTTON3BUTTON)) {
    Serial.println("Button 3 pressed!");
    Keyboard.print(currentMillis/10);
    Keyboard.println(" c");
  }
  if (intStatus & (1 << SX1509_BUTTON4BUTTON)) {
    Serial.println("Button 4 pressed!");
    Keyboard.print(currentMillis/10);
    Keyboard.println(" d");
  }
  if (intStatus & (1 << SX1509_BUTTON5BUTTON)) {
    Serial.println("Button 5 pressed!");
    Keyboard.print(currentMillis/10);
    Keyboard.println(" e");
  }
  if (intStatus & (1 << SX1509_BUTTON6BUTTON)) {
    Serial.println("Button 6 pressed!");
    Keyboard.print(currentMillis/10);
    Keyboard.println(" f");
  }
  if (intStatus & (1 << SX1509_BUTTON7BUTTON)) {
    Serial.println("Button 7 pressed!");
    Keyboard.print(currentMillis/10);
    Keyboard.println(" g");
  }
  if (intStatus & (1 << SX1509_BUTTON8BUTTON)) {
    Serial.println("Button 8 pressed!");
    io.digitalWrite(SX1509_BUTTON8LED, HIGH);
    Keyboard.print(currentMillis/10);
    Keyboard.println(" h");
  }
}

void loop() {
  currentMillis = millis();
  Serial.print(".");
  if  (buttonPushed) {
    buttonPushed = 0;
    doMultiplexedButtons(); }

  if (currentMillis - lastAction > 1000) {
    analogWrite(16, 0);    
    Serial.println("huh");
  } else if (currentMillis - lastAction > 500) {
    Serial.println("jaja");
    analogWrite(16, 4);
  }
  
  if (button0.fallingEdge()) {
    //Keyboard.print(currentMillis/10);
    //Keyboard.println(" a");
    lastAction = currentMillis;
    analogWrite(16, 160);
  }

}