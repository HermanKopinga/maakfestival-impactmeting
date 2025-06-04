#include <Arduino.h>
#include <Bounce.h>

Bounce button0 = Bounce(23, 20);
Bounce button1 = Bounce(22, 20);  // 10 ms debounce time is appropriate
Bounce button2 = Bounce(21, 20);  // for most mechanical pushbuttons
Bounce button3 = Bounce(20, 20);  // These feel noisy, 20ms it is.
Bounce button4 = Bounce(19, 20);

unsigned long lastAction = 0;
unsigned long currentMillis = 0;

void setup() {
  pinMode(23, INPUT_PULLUP);
  pinMode(22, INPUT_PULLUP);
  pinMode(21, INPUT_PULLUP);
  pinMode(20, INPUT_PULLUP);
  pinMode(19, INPUT_PULLUP);
  Keyboard.begin();
  pinMode(16, OUTPUT);  
  Serial.begin(115200);
}

void loop() {
  currentMillis = millis();
  button0.update();
  button1.update();    
  button2.update();
  button3.update();  
  button4.update();  
  
  if (currentMillis - lastAction > 1000) {
    analogWrite(16, 0);    
    Serial.println("huh");
  } else if (currentMillis - lastAction > 500) {
    Serial.println("jaja");
    analogWrite(16, 4);
  }
  
  if (button0.fallingEdge()) {
    Keyboard.print(currentMillis/10);
    Keyboard.println(" a");
    lastAction = currentMillis;
    analogWrite(16, 160);
  }
  if (button1.fallingEdge()) {
    Keyboard.print(currentMillis/10);
    Keyboard.println(" b");
    lastAction = currentMillis;
    digitalWrite(16, 160);
  }
  if (button2.fallingEdge()) {
    Keyboard.print(currentMillis/10);
    Keyboard.println(" c");
    lastAction = currentMillis;
    analogWrite(16, 160);
  }
  if (button3.fallingEdge()) {
    Keyboard.print(currentMillis/10);
    Keyboard.println(" d");
    lastAction = currentMillis;
    analogWrite(16, 160);
  }
  if (button4.fallingEdge()) {
    Keyboard.print(currentMillis/10);
    Keyboard.println(" e");
    lastAction = currentMillis;
    analogWrite(16, 160);
  }
}
