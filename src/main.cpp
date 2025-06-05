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


const byte BUTTON1BUTTON = 28;
const byte BUTTON2BUTTON = 28;
const byte BUTTON3BUTTON = 28;
const byte BUTTON4BUTTON = 28;
const byte BUTTON5BUTTON = 28;
const byte BUTTON6BUTTON = 28;
const byte BUTTON7BUTTON = 28;
const byte BUTTON8BUTTON = 27;
const byte BUTTON9BUTTON = 26;
const byte BUTTON10BUTTON = 28;

const byte BUTTON1LED = 25;
const byte BUTTON2LED = 25;
const byte BUTTON3LED = 25;
const byte BUTTON4LED = 25;
const byte BUTTON5LED = 25;
const byte BUTTON6LED = 25;
const byte BUTTON7LED = 25;
const byte BUTTON8LED = 25;
const byte BUTTON9LED = 25;
const byte BUTTON10LED = 25;

const byte SX1509_INTERRUPT_PIN = 20;

#define sdaPin 18
#define sclPin 19
SX1509 io;

Bounce button0 = Bounce(BUTTON1BUTTON, 20);
Bounce button1 = Bounce(BUTTON2BUTTON, 20);  // 10 ms debounce time is appropriate
Bounce button2 = Bounce(BUTTON3BUTTON, 20);  // for most mechanical pushbuttons
Bounce button3 = Bounce(BUTTON4BUTTON, 20);  // These feel noisy, 20ms it is.
Bounce button4 = Bounce(BUTTON5BUTTON, 20);
Bounce button5 = Bounce(BUTTON6BUTTON, 20);
Bounce button6 = Bounce(BUTTON7BUTTON, 20);
Bounce button7 = Bounce(BUTTON8BUTTON, 20);
Bounce button8 = Bounce(BUTTON9BUTTON, 20);
Bounce button9 = Bounce(BUTTON10BUTTON, 20);

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

// Helper function to map LED number to SX1509_BUTTONxLED constant
int getLedConstant(int ledNumber) {
  switch (ledNumber) {
    case 1: return SX1509_BUTTON1LED;
    case 2: return SX1509_BUTTON2LED;
    case 3: return SX1509_BUTTON3LED;
    case 4: return SX1509_BUTTON4LED;
    case 5: return SX1509_BUTTON5LED;
    case 6: return SX1509_BUTTON6LED;
    case 7: return SX1509_BUTTON7LED;
    case 8: return SX1509_BUTTON8LED;
    default: return -1;
  }
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

  // Blink the LEDs a few times before we start:
  for (int i = 0; i < 4; i++) {
    // Walk LEDs from 1 to 8
    for (int i = 1; i <= 8; i++) {
      // Turn off previous LED (if not the first)
      if (i > 1) {
        io.digitalWrite(getLedConstant(i - 1), LOW);
      } else {
        io.digitalWrite(getLedConstant(8), LOW); // wrap around
      }

      io.digitalWrite(getLedConstant(i), HIGH);

      delay(40);
    }
  }
  io.digitalWrite(SX1509_BUTTON8LED, LOW);
  //io.pinMode(SX1509_WHITELED1, ANALOG_OUTPUT);
}


void setup() {
  pinMode(BUTTON1BUTTON, INPUT_PULLUP);
  pinMode(BUTTON2BUTTON, INPUT_PULLUP);
  pinMode(BUTTON3BUTTON, INPUT_PULLUP);
  pinMode(BUTTON4BUTTON, INPUT_PULLUP);
  pinMode(BUTTON5BUTTON, INPUT_PULLUP);
  pinMode(BUTTON6BUTTON, INPUT_PULLUP);
  pinMode(BUTTON7BUTTON, INPUT_PULLUP);
  pinMode(BUTTON8BUTTON, INPUT_PULLUP);
  pinMode(BUTTON9BUTTON, INPUT_PULLUP);
  pinMode(BUTTON10BUTTON, INPUT_PULLUP);
  
  pinMode(BUTTON1LED, OUTPUT);
  pinMode(BUTTON2LED, OUTPUT);
  pinMode(BUTTON3LED, OUTPUT);
  pinMode(BUTTON4LED, OUTPUT);
  pinMode(BUTTON5LED, OUTPUT);
  pinMode(BUTTON6LED, OUTPUT);
  pinMode(BUTTON7LED, OUTPUT);
  pinMode(BUTTON8LED, OUTPUT);
  pinMode(BUTTON9LED, OUTPUT);
  pinMode(BUTTON10LED, OUTPUT);
  
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
    io.digitalWrite(SX1509_BUTTON7LED, HIGH);
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

  button0.update();
  button1.update();    
  button2.update();
  button3.update();  
  button4.update(); 
  button5.update();
  button6.update();
  button7.update();
  button8.update();
  button9.update();

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
    Keyboard.print(currentMillis/10);
    Keyboard.println(" 1");
    lastAction = currentMillis;
    analogWrite(BUTTON1LED, 160);
  }
  if (button1.fallingEdge()) {
    Keyboard.print(currentMillis/10);
    Keyboard.println(" 2");
    lastAction = currentMillis;
    analogWrite(BUTTON2LED, 160);
  }
  if (button2.fallingEdge()) {
    Keyboard.print(currentMillis/10);
    Keyboard.println(" 3");
    lastAction = currentMillis;
    analogWrite(BUTTON3LED, 160);
  }
  if (button3.fallingEdge()) {
    Keyboard.print(currentMillis/10);
    Keyboard.println(" 4");
    lastAction = currentMillis;
    analogWrite(BUTTON4LED, 160);
  }
  if (button4.fallingEdge()) {
    Keyboard.print(currentMillis/10);
    Keyboard.println(" 5");
    lastAction = currentMillis;
    analogWrite(BUTTON5LED, 160);
  }
  if (button5.fallingEdge()) {
    Keyboard.print(currentMillis/10);
    Keyboard.println(" 6");
    lastAction = currentMillis;
    analogWrite(BUTTON6LED, 160);
  }
  if (button6.fallingEdge()) {
    Keyboard.print(currentMillis/10);
    Keyboard.println(" 7");
    lastAction = currentMillis;
    analogWrite(BUTTON7LED, 160);
  }
  if (button7.fallingEdge()) {
    Keyboard.print(currentMillis/10);
    Keyboard.println(" 8");
    lastAction = currentMillis;
    analogWrite(BUTTON8LED, 160);
  }
  if (button8.fallingEdge()) {
    Keyboard.print(currentMillis/10);
    Keyboard.println(" 9");
    lastAction = currentMillis;
    analogWrite(BUTTON9LED, 160);
  }
  if (button9.fallingEdge()) {
    Keyboard.print(currentMillis/10);
    Keyboard.println(" 10");
    lastAction = currentMillis;
    analogWrite(BUTTON10LED, 160);
  }
}