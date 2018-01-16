/********
    First midi controller test with a single button! :D
    
    
*********/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// These are for using software SPI
#define OLED_MOSI    9
#define OLED_CLK     10
#define OLED_DC      11
#define OLED_CS      12
#define OLED_RESET      13
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// This is for the potentiometer
const int analogPin = A0;

// The button
const int buttonPin = 2;
int buttonState;
int lastButtonState = LOW;
boolean var1 = false;

// These help with debouncing
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 20;

void setup()  {
  
  // Initialize the button
  pinMode(buttonPin, INPUT);
  
  Serial.begin(31250);
  
  display.begin(SSD1306_SWITCHCAPVCC);
  
  
  display.display();
  delay(3);
  display.clearDisplay();
  
  
  // Some text
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(15,20);
  display.println("MIDI\nController\nTest 1");
  display.display();
  delay(2000);
  display.clearDisplay();
  
  display.setTextSize(1);
  
}

void loop() {
  
  int numBars = 14;  // total number of bars to use on display
  
  int sensorReading = analogRead(analogPin);
  
  int level = map(sensorReading, 0, 1021, 100, 0);      // POT mapped from 0-100
  int bar = map(sensorReading, 0, 1021, numBars, 0);    // number of bars for the current level
  int note = map(sensorReading, 0, 1021, 91, 31);
  
  buttonState = digitalRead(buttonPin);
  if(buttonState != lastButtonState)
    lastDebounceTime = millis();
  if((millis() - lastDebounceTime) > debounceDelay){
    if(buttonState == HIGH){
      Serial.write(0x90);  // Note on channel 1
      Serial.write(note);  // Which note to write (based on potentiometer)
      Serial.write(0x45);  // Velocity (in this case, about the middle)
    }
  }
  lastButtonState = buttonState;
  
  
  // Display output:
  
  display.setCursor(0,25);
  display.print("[");
  for(int i = 0; i < bar; i++){
    display.print("|");
  }
  for(int i = 0; i < numBars-bar; i++){
    display.print(" ");
  }
  display.print("] ");
  display.print(level);
  display.println("%");
  if( level == 100 ){
    display.println("\n\nMaximum level!");
  }
  else if ( level < 1 ){
    display.println("\n\nMinimum level!");
  }
  display.display();
  delay(10);
  display.clearDisplay();
  
}
