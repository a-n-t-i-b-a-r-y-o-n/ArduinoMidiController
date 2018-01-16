/********

    VERSION 0.6

    MIDI info from here:
    http://hinton-instruments.co.uk/reference/midi/protocol/index.htm
    http://www.indiana.edu/~emusic/cntrlnumb.html
    
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

// The button
const int buttonPin = 2;
boolean status = false;
int currentnote = 0;
int prevPitch = 64;

// Transmission symbol
int transmit = 0;

void setup()  {

  Serial.begin(31250);
  
  // Initialize the button
  pinMode(buttonPin, INPUT);
  

  display.begin(SSD1306_SWITCHCAPVCC);

  display.clearDisplay();
  
  // Some text
  /*
  display.setTextSize(2);
  display.setTextColor(BLACK, WHITE);
  display.println("          ");
  display.println("  Anti-   \n  Matter  ");
  display.println("          ");
  display.display();
  delay(1000);
  display.clearDisplay();
  */
  
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);


  Serial.write(0x90);
    Serial.write(36);
    Serial.write(127);

    Serial.write(0xB0);
    Serial.write(0x40);
    Serial.write(65);

  
}

void loop() {
  
  
  int numBars = 14;  // total number of bars to use on display

  int channel = map(analogRead(A4), 15, 940, 15, 0);
  channel = (channel < 0 ? 0 : channel);
  channel = (channel > 15 ? 15 : channel);

  int general = map(analogRead(A3), 15, 940, 127, 0);
  general = (general < 0 ? 0 : general);
  general = (general > 127 ? 127 : general);

  int pitch = map(analogRead(A2), 15, 940, 0x80, 0);
  pitch = (pitch < 0 ? 0 : pitch);
  pitch = (pitch > 0x80 ? 0x80 : pitch);

  int velocity = map(analogRead(A1), 15, 940, 127, 0);        // Actual midi velocity
  // Fix for dead zones:
  velocity = (velocity < 0 ? 0 : velocity);
  velocity = (velocity > 127 ? 127 : velocity);
  int velPercent = map(velocity, 127, 0, 100, 1);      // Velocity percentage
  int velBars = map(velocity, 127, 0, numBars, 0);     // number of bars to display

  
  int note = (status ? currentnote : map(analogRead(A0), 15, 940, 127, 0));          // Current note
  // Fix for dead zones:
  note = (note < 0 ? 0 : note);
  note = (note > 127 ? 127 : note);
  int noteBars = map(note, 0, 127, 0, numBars);  // number of bars to display
  String notes[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };


  // BUTTONS

  bool b1 = (digitalRead(2) == HIGH ? true : false);

  if(b1 && !status){

    // Initial General
    
    
    // Initial Press
    Serial.write(0x90 + channel);
    Serial.write(note);
    Serial.write(velocity);
    
    status = true;
  }
  else if(b1 && status){
    // Sustain
    Serial.write(0xB0 + channel);
    Serial.write(0x40);
    Serial.write(65);

    
    // General

    
  }
  else if(!b1 && status){
    // Kill Note
    Serial.write(0x90 + channel);
    Serial.write(note);
    Serial.write(0x00);
    // Stop Sustain
    Serial.write(0xB0 + channel);
    Serial.write(0x40);
    Serial.write(63);

    
    status = false;
  }
  else if(!b1){
    status = false;
  }


  bool b2 = (digitalRead(3) == HIGH ? true : false);

  if(b2){
    Serial.write(0xB0 + channel);
    Serial.write(0x7B);
    Serial.write(0x65);
  }


  // PITCH BEND
  
  if(pitch != prevPitch){
    Serial.write(0xE0 + channel);
    Serial.write(0x00);
    Serial.write(pitch);
    prevPitch = pitch;
  }  


  // GENERAL KNOB

  Serial.write(0xB0);
  Serial.write(0x50);
  Serial.write(general);

  
  
  // DISPLAY
  
  display.setCursor(0, 0);

  // MIDI State

  display.print("MIDI State:  ");
  String symbols[] = {"-", "\\", "|", "/" };
  if(status){
    transmit++;
    display.println(symbols[((transmit/10)%4)]);
  }
  else  
    display.println("- ");  

  // Display the Channel info

  display.print("\nPitch:  ");
  display.println(pitch);
  
  
  // Display the note info:
  
  display.println("Note:");
  display.print("[");
  for(int i = 0; i < noteBars-1; i++)
    display.print("-");
  
  display.setTextColor(BLACK, WHITE);
  display.print(" ");
  display.setTextColor(WHITE, BLACK);
  for(int i = 0; i < numBars-(noteBars == 0 ? 1 : noteBars); i++)
    display.print("-");
  
  display.print("] ");
  display.print(notes[note%12]);
  display.println(note/12);
  
  
  // Display the velocity info:
  
  display.println("Velocity:");
  display.print("[");
  display.setTextColor(BLACK, WHITE);
  for(int i = 0; i < velBars; i++)
    display.print(" ");
  display.setTextColor(WHITE, BLACK);
  
  for(int i = 0; i < numBars-velBars; i++)
    display.print(" ");
    
  display.print("] ");
  display.print(velPercent);
  display.println("%");


  // Print to display
    
  display.display();
  delay(5);
  display.clearDisplay();


  currentnote = note;

  prevPitch = pitch;

}

