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

const byte BUTTON1BUTTON = 7;
const byte BUTTON2BUTTON = 8;
const byte BUTTON3BUTTON = 30;
const byte BUTTON4BUTTON = 29;
const byte BUTTON5BUTTON = 24;
const byte BUTTON6BUTTON = 14;
const byte BUTTON7BUTTON = 28;
const byte BUTTON8BUTTON = 27;
const byte BUTTON9BUTTON = 26;
const byte BUTTON10BUTTON = 11;

const byte BUTTON1LED = 3;
const byte BUTTON2LED = 23;
const byte BUTTON3LED = 22;
const byte BUTTON4LED = 21;
const byte BUTTON5LED = 25;
const byte BUTTON6LED = 5;
const byte BUTTON7LED = 32;
const byte BUTTON8LED = 6;
const byte BUTTON9LED = 9;
const byte BUTTON10LED = 10;

const byte BUTTONGOBUTTON = 12;
const byte BUTTONGOLED = 4;

const byte BUTTONQ21BUTTON = 33;
const byte BUTTONQ22BUTTON = 17;
const byte BUTTONQ23BUTTON = 31;
const byte BUTTONQ21LED = 16;
const byte BUTTONQ22LED = 15;
const byte BUTTONQ23LED = 13;

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
Bounce buttongo = Bounce(BUTTONGOBUTTON, 20);
Bounce buttonq21 = Bounce(BUTTONQ21BUTTON, 20);
Bounce buttonq22 = Bounce(BUTTONQ22BUTTON, 20);
Bounce buttonq23 = Bounce(BUTTONQ23BUTTON, 20);

unsigned long lastAction = 0;
unsigned long currentMillis = 0;

byte buttonPushed = 0;

const byte numberOfButtons = 22;
const byte sourceSX1509 = 1;
const byte sourceDirect = 0;

// Store state in couple of arrays.
byte pInfoActive[numberOfButtons];
byte pInfoPin[numberOfButtons] = {SX1509_BUTTON1LED,SX1509_BUTTON2LED,SX1509_BUTTON3LED,SX1509_BUTTON4LED,SX1509_BUTTON5LED,SX1509_BUTTON6LED,SX1509_BUTTON7LED,SX1509_BUTTON8LED,BUTTONQ21LED,BUTTONQ22LED,BUTTONQ23LED,BUTTON1LED,BUTTON2LED,BUTTON3LED,BUTTON4LED,BUTTON5LED,BUTTON6LED,BUTTON7LED,BUTTON8LED,BUTTON9LED,BUTTON10LED,BUTTONGOLED};
byte pInfoSource[numberOfButtons] = {sourceSX1509,sourceSX1509,sourceSX1509,sourceSX1509,sourceSX1509,sourceSX1509,sourceSX1509,sourceSX1509,sourceDirect,sourceDirect,sourceDirect,sourceDirect,sourceDirect,sourceDirect,sourceDirect,sourceDirect,sourceDirect,sourceDirect,sourceDirect,sourceDirect,sourceDirect,sourceDirect};
char pInfoOutput[numberOfButtons] = {'a','b','c','d','e','f','g','h','.','>','?','1','2','3','4','5','6','7','8','9','x','!'};

void turnLedOn (byte position) {
  pInfoActive[position] = 1;
  if (pInfoSource[position] == sourceSX1509) {
    io.digitalWrite(pInfoPin[position], HIGH);
  } else {
    digitalWrite(pInfoPin[position], 255);
  }
}

void turnLedOff (byte position) {
  pInfoActive[position] = 0;
  if (pInfoSource[position] == sourceSX1509) {
    io.digitalWrite(pInfoPin[position], LOW);
  } else {
    digitalWrite(pInfoPin[position], 0);
  }  
}

byte whatQuestion (byte position) {
  if (position <= 7) {
    return 1;
  } else if (position <= 10) {
    return 2;
  } else if (position <= 20) {
    return 3;
  } else {
    return 0; // not good :)
  }
}

// Functie die een hoop werk doet voor de 'logica' van het ding.
// Wil dat hij na elke knopdruk controleert of 
// Vraag 1 correct is (1 of meer antwoorden)
// Vraag 2 correct, 1 antwoord
// Vraag 3 correct, 1 antwoord.
// Als dat allemaal klopt led 21 aanzetten.
byte checkQuestion(byte position) {
  byte answerCount[] = {0,0,0,0};

  // Eerste vraag, meerdere mogelijk.
  for (byte i=0;i<=7;i++) {
    if (pInfoActive[i]) {
      answerCount[1]++;
    }
  }
  
  // Tweede vraag, eentje mogelijk.
  for (byte i=8;i<=10;i++) {
    if (whatQuestion(position) == 2) {
      pInfoActive[i] = 0;
      turnLedOff(i);
    }
    if (pInfoActive[i]) {
      answerCount[2]++;
    }
  }

  // Cijfer, eentje mogelijk.
  for (byte i=11;i<=20;i++) {
    if (whatQuestion(position) == 3) {
      pInfoActive[i] = 0;
      turnLedOff(i);
    }
    if (pInfoActive[i]) {
      answerCount[3]++;
    }
  }
  
  answerCount[whatQuestion(position)]++;
 
  if (answerCount[1] > 0 && answerCount[2] == 1 && answerCount[3] == 1) {
    turnLedOn(21);
    Serial.print("Good: ");
    Serial.print(answerCount[1]);
    Serial.print(" ");
    Serial.print(answerCount[2]);
    Serial.print(" ");
    Serial.println(answerCount[3]);
    return 1;
  } else {
    turnLedOff(21);
    Serial.print("Bad: ");
    Serial.print(answerCount[1]);
    Serial.print(" ");
    Serial.print(answerCount[2]);
    Serial.print(" ");
    Serial.println(answerCount[3]);
    return 0;
  }
}

void disco() {
  // Blink the LEDs a few times before we start:
  for (int i = 0; i < 4; i++) {
    for (int i = 0; i < numberOfButtons; i++) {
      // Turn off previous LED (if not the first)
      if (i > 0) {
        turnLedOff(i-1);
      } else {
        turnLedOff(21); // wrap around
      }
      turnLedOn(i);
      delay(10);
    }
  }
  turnLedOff(21); // wrap around
}

void printState() {
  for (byte i = 0; i < numberOfButtons; i++) {
    Serial.print(" ");
    Serial.print(pInfoActive[i]);
  }
  Serial.println();
}

void processPress(byte position, const char* output) {
  checkQuestion(position);
  if (pInfoActive[position]) {
    turnLedOff(position);
    pInfoActive[position] = 0;
    Keyboard.print(millis()/10);
    Keyboard.print("-");
    Keyboard.println(output);
    Serial.print("Button ");
    Serial.print(position);
    Serial.print(" pressed, deactivated ");
    Serial.print(output);
    printState();
  } else {
    turnLedOn(position);
    pInfoActive[position] = 1;
    Keyboard.print(millis()/10);
    Keyboard.print(" ");
    Keyboard.println(output);
    Serial.print("Button ");
    Serial.print(position);
    Serial.print(" pressed, output: ");
    Serial.print(output);
    printState();
  }
}

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
    digitalWrite(3, HIGH);  // If we failed to communicate, turn the pin 13 LED on
    digitalWrite(22, HIGH);  // If we failed to communicate, turn the pin 22 LED on
    while (1) {
      digitalWrite(13, 1);
      delay(200);
      digitalWrite(13, 0);
      delay(200);
    }
  }

  io.debounceTime(20);  // 10ms is default, 20 is a bit more noise compatible.

  io.pinMode(SX1509_BUTTON1LED, ANALOG_OUTPUT);
  io.pinMode(SX1509_BUTTON1BUTTON, INPUT_PULLUP);
  io.pinMode(SX1509_BUTTON2LED, ANALOG_OUTPUT);
  io.pinMode(SX1509_BUTTON2BUTTON, INPUT_PULLUP);
  io.pinMode(SX1509_BUTTON3LED, ANALOG_OUTPUT);
  io.pinMode(SX1509_BUTTON3BUTTON, INPUT_PULLUP);
  io.pinMode(SX1509_BUTTON4LED, ANALOG_OUTPUT);
  io.pinMode(SX1509_BUTTON4BUTTON, INPUT_PULLUP);
  io.pinMode(SX1509_BUTTON5LED, ANALOG_OUTPUT);
  io.pinMode(SX1509_BUTTON5BUTTON, INPUT_PULLUP);
  io.pinMode(SX1509_BUTTON6LED, ANALOG_OUTPUT);
  io.pinMode(SX1509_BUTTON6BUTTON, INPUT_PULLUP);
  io.pinMode(SX1509_BUTTON7LED, ANALOG_OUTPUT);
  io.pinMode(SX1509_BUTTON7BUTTON, INPUT_PULLUP);
  io.pinMode(SX1509_BUTTON8LED, ANALOG_OUTPUT);
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
  pinMode(BUTTONGOBUTTON, INPUT_PULLUP);
  pinMode(BUTTONQ21BUTTON, INPUT_PULLUP);
  pinMode(BUTTONQ22BUTTON, INPUT_PULLUP);
  pinMode(BUTTONQ23BUTTON, INPUT_PULLUP);
  
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
  pinMode(BUTTONGOLED, OUTPUT);
  pinMode(BUTTONQ21LED, OUTPUT);
  pinMode(BUTTONQ22LED, OUTPUT);
  pinMode(BUTTONQ23LED, OUTPUT);
  
  
  pinMode(13, OUTPUT);
  digitalWrite(13, 1);
  
  Keyboard.begin();
  
  Serial.begin(115200);

  multiplexerSetup();
  Serial.println("Maakfestival Impact monitor door Herman Kopinga (herman@kopinga.nl)");
  
  // Make sure all leds work, turn them on at start.
  for (int i = 0; i <= 21; i++) {
    turnLedOn(i);
  }
  delay(500);
  // Hidden debug feature if 10 is pushed at the start keep showing the LEDS.
  while (digitalRead(BUTTON10BUTTON) == 0) {
    delay(200);
  }
  for (int i = 0; i <= 21; i++) {
    turnLedOff(i);
  }
  digitalWrite(13, 0);
}

void doMultiplexedButtons() {
  // read io.interruptSource() find out which pin generated
  // an interrupt and clear the SX1509's interrupt output.
  unsigned int intStatus = io.interruptSource();
  // For debugging handiness, print the intStatus variable.
  // Each bit in intStatus represents a single SX1509 IO.
  Serial.println("intStatus = " + String(intStatus, BIN));

  if (intStatus & (1 << SX1509_BUTTON1BUTTON)) {
    processPress(0, "a"); 
  }
  if (intStatus & (1 << SX1509_BUTTON2BUTTON)) {
    processPress(1, "b");  
  }
  if (intStatus & (1 << SX1509_BUTTON3BUTTON)) {
    processPress(2, "c");         
  }
  if (intStatus & (1 << SX1509_BUTTON4BUTTON)) {
    processPress(3, "d");  
  }
  if (intStatus & (1 << SX1509_BUTTON5BUTTON)) {
    processPress(4, "e");   
  }
  if (intStatus & (1 << SX1509_BUTTON6BUTTON)) {
    processPress(5, "f");    
  }
  if (intStatus & (1 << SX1509_BUTTON7BUTTON)) {
    processPress(6, "g");  
  }
  if (intStatus & (1 << SX1509_BUTTON8BUTTON)) {
    processPress(7, "h"); 
  }
}

void loop() {
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
  buttongo.update();
  buttonq21.update();
  buttonq22.update();
  buttonq23.update();

  if  (buttonPushed) {
    buttonPushed = 0;
    doMultiplexedButtons(); 
  }

  if (buttonq21.fallingEdge()) {
    processPress(8, ".");
  }
  if (buttonq22.fallingEdge()) {
    processPress(9, ">");
  }
  if (buttonq23.fallingEdge()) {
    processPress(10, "?");
  }
  if (button0.fallingEdge()) {
    processPress(11, "1");
  }
  if (button1.fallingEdge()) {
    processPress(12, "2");     
  }
  if (button2.fallingEdge()) {
    processPress(13, "3");     
  }
  if (button3.fallingEdge()) {
    processPress(14, "4");     
  }
  if (button4.fallingEdge()) {
    processPress(15, "5");     
  }
  if (button5.fallingEdge()) {
    processPress(16, "6");     
  }
  if (button6.fallingEdge()) {
    processPress(17, "7");     
  }
  if (button7.fallingEdge()) {
    processPress(18, "8");     
  }
  if (button8.fallingEdge()) {
    processPress(19, "9");     
  }
  if (button9.fallingEdge()) {
    processPress(20, "x");    
  }
  if (buttongo.fallingEdge()) {
    processPress(21, "!");    
    // Hele status van de knoppen sturen.
    Keyboard.print(millis());
    Keyboard.print(" # ");
    for (byte i = 0; i < numberOfButtons; i++) {
      if (pInfoActive[i]) {  
        Keyboard.print(pInfoOutput[i]);
      }
    }
    Keyboard.println();
    disco();
  }
}