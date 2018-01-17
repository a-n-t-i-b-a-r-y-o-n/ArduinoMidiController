/********

    VERSION 0.9

    MIDI info from here:
    http://hinton-instruments.co.uk/reference/midi/protocol/index.htm
    http://www.indiana.edu/~emusic/cntrlnumb.html
    
    This is basically an extended, optimized version of MidiController1 version 0.6 that features multiple modes.


    TODO:
      - Debounce b3 ?
      - Bug tests?
    
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


int mode = 2;


// The buttons
boolean status = false;   // Note transmission status
int currentNote = 0;      // M1: used to ensure we don't get stuck sending a note forever  M2: Current note to broadcast
int prevPitch = 64;          // previous loop pitch value

// Transmission symbol
int transmit = 0;

void setup()  {

  Serial.begin(31250);
  
  // Initialize the buttons
  for (int i = 2; i < 5; i++)
    pinMode(i, INPUT);
  
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  
  // Startup text
  display.setTextSize(2);
  display.setTextColor(BLACK, WHITE);
  display.println("          ");
  display.println("  Anti-   \n  Matter  ");
  display.println("          ");
  display.display();
  delay(1000);
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);


  /* RGB LED mapping:
    8 - R
    7 - G
    6 - B
  */

  for(int i = 6; i < 8; i++)
    pinMode(i, OUTPUT);
  
}

void loop() {
  
  display.setCursor(0,0);

  int numBars = 14;  // total number of bars to use on display

  bool b0 = (digitalRead(2) == HIGH ? true : false);
  bool b1 = (digitalRead(3) == HIGH ? true : false);
  bool b2 = (digitalRead(4) == HIGH ? true : false);
  bool b3 = (digitalRead(5) == HIGH ? true : false);

  /* BUTTON MAPPING
    2 - Send note
    3 - Lower note (mode 2 only)
    4 - Higher note (mode 2 only)
    5 - Change Mode
  */

  if(b3)
    mode = (mode == 1 ? 2 : 1);



  if(mode == 1){

    digitalWrite(7, LOW);
    digitalWrite(6, HIGH);

    /* Potentiometer mapping for mode 1:
      PIN  VAR  MEANING
      A0   p0   Note Value
      A1   p1   Velocity
      A2   p2   Pitch bend
      A3   p3   General
      A4   p4   Channel

    */

    int p4 = map(analogRead(A4), 15, 940, 15, 0);
    p4 = (p4 < 0 ? 0 : p4);
    p4 = (p4 > 15 ? 15 : p4);

    int p3 = map(analogRead(A3), 15, 940, 127, 0);
    p3 = (p3 < 0 ? 0 : p3);
    p3 = (p3 > 127 ? 127 : p3);

    int p2 = map(analogRead(A2), 15, 940, 0x80, 0);
    p2 = (p2 < 0 ? 0 : p2);
    p2 = (p2 > 0x80 ? 0x80 : p2);

    int p1 = map(analogRead(A1), 15, 940, 127, 0);
    p1 = (p1 < 0 ? 0 : p1);
    p1 = (p1 > 127 ? 127 : p1);

    
    int p0 = (status ? currentNote : map(analogRead(A0), 15, 940, 127, 0));
    p0 = (p0 < 0 ? 0 : p0);
    p0 = (p0 > 127 ? 127 : p0);
    String notes[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };


    // BUTTONS

    if(b0 && !status){
      
      
      // Initial Press
      Serial.write(0x90 + p4);
      Serial.write(p0);
      Serial.write(p1);
      
      status = true;
    }
    else if(b0 && status){
      // Sustain
      Serial.write(0xB0 + p4);
      Serial.write(0x40);
      Serial.write(65);
      
    }
    else if(!b0 && status){
      // Kill note
      Serial.write(0x90 + p4);
      Serial.write(p0);
      Serial.write(0x00);
      // Stop Sustain
      Serial.write(0xB0 + p4);
      Serial.write(0x40);
      Serial.write(63);

      
      status = false;
      transmit = 0;
    }
    else if(!b0){
      status = false;
    }


    bool b2 = (digitalRead(3) == HIGH ? true : false);

    if(b2){
      Serial.write(0xB0 + p4);
      Serial.write(0x7B);
      Serial.write(0x65);
    }


    // pitch BEND
    
    if(p2 != prevPitch){
      Serial.write(0xE0 + p4);
      Serial.write(0x00);
      Serial.write(p2);
      prevPitch = p2;
    }  


    // General KNOB

    Serial.write(0xB0);
    Serial.write(0x50);
    Serial.write(p3);

    
    
    // DISPLAY

    // MIDI State

    if(status){
      transmit++;
      display.print("                 .");
      for(int i = 0; i < ((transmit/10)%4); i++)
        display.print(".");
    }
    display.println("");  

    // Display pitch bend info

    display.print("\nPitch Bend:  ");
    display.print(p2 > 64 ? "+" : "");
    display.println(p2-64);
    
    
    // Display the note info:
    
    display.println("Note:");
    display.print("[");
    for(int i = 0; i < map(p0, 0, 127, 0, numBars)-1; i++)
      display.print("-");
    
    display.setTextColor(BLACK, WHITE);
    display.print(" ");
    display.setTextColor(WHITE, BLACK);
    for(int i = 0; i < numBars-(map(p0, 0, 127, 0, numBars) == 0 ? 1 : map(p0, 0, 127, 0, numBars)); i++)
      display.print("-");
    
    display.print("] ");
    display.print(notes[p0%12]);
    display.println(p0/12);
    
    
    // Display the velocity info:
    
    display.println("Velocity:");
    display.print("[");
    display.setTextColor(BLACK, WHITE);
    for(int i = 0; i < map(p1, 127, 0, numBars, 0); i++)
      display.print(" ");
    display.setTextColor(WHITE, BLACK);
    
    for(int i = 0; i < numBars-map(p1, 127, 0, numBars, 0); i++)
      display.print(" ");
      
    display.print("] ");
    display.print(map(p1, 127, 0, 100, 1));
    display.println("%");

    currentNote = p0;
    prevPitch = p2;
  }
  else if(mode == 2){

    digitalWrite(6, LOW);
    digitalWrite(7, HIGH);


    int p0 = map(analogRead(A0), 15, 940, 127, 0);
    p0 = (p0 < 0 ? 0 : p0);
    p0 = (p0 > 127 ? 127 : p0);
    
    int p1 = map(analogRead(A1), 15, 940, 127, 0);
    p1 = (p1 < 0 ? 0 : p1);
    p1 = (p1 > 127 ? 127 : p1);
    
    int p2 = map(analogRead(A2), 15, 940, 127, 0);
    p2 = (p2 < 0 ? 0 : p2);
    p2 = (p2 > 127 ? 127 : p2);
    
    int p3 = map(analogRead(A3), 15, 940, 127, 0);
    p3 = (p3 < 0 ? 0 : p3);
    p3 = (p3 > 127 ? 127 : p3);
    
    int p4 = map(analogRead(A4), 15, 940, 127, 0);
    p4 = (p4 < 0 ? 0 : p4);
    p4 = (p4 > 127 ? 127 : p4);


    if(b0 && !status){

      // Initial Press
      Serial.write(0x90);
      Serial.write(currentNote);
      Serial.write(0x7F);
      
      status = true;
    }
    else if(b0 && status){
      // Sustain
      Serial.write(0xB0);
      Serial.write(0x40);
      Serial.write(65);

      
    }
    else if(!b0 && status){
      // Kill note
      Serial.write(0x90);
      Serial.write(currentNote);
      Serial.write(0x00);
      // Stop Sustain
      Serial.write(0xB0);
      Serial.write(0x40);
      Serial.write(63);

      
      status = false;
      transmit = 0;
    }
    else if(!b0){
      status = false;
    }
    
    
    if(b1)
      currentNote--;
      
    if(b2)
      currentNote++;


    // MIDI State

    if(status){
      transmit++;
      display.print("                 .");
      for(int i = 0; i < ((transmit/10)%4); i++)
        display.print(".");
    }
    display.println("");

    // Display the current note
    String notes[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    display.print("[");
    for(int i = 0; i < map(currentNote, 0, 127, 0, numBars)-1; i++)
      display.print("-");
    
    display.setTextColor(BLACK, WHITE);
    display.print(" ");
    display.setTextColor(WHITE, BLACK);
    for(int i = 0; i < numBars-(map(currentNote, 0, 127, 0, numBars) == 0 ? 1 : map(currentNote, 0, 127, 0, numBars)); i++)
      display.print("-");
    
    display.print("] ");
    display.print(notes[currentNote%12]);
    display.println(currentNote/12);


    // Display the 5 potentiometers' output
    display.print("\n[");
    display.setTextColor(BLACK, WHITE);
    for(int i = 0; i < map(p0, 127, 0, numBars, 0); i++)
      display.print(" ");
    display.setTextColor(WHITE, BLACK);
    
    for(int i = 0; i < numBars-map(p0, 127, 0, numBars, 0); i++)
      display.print(" ");
      
    display.print("] ");
    display.print(map(p0, 127, 0, 100, 1));
    display.println("%");

    display.print("[");
    display.setTextColor(BLACK, WHITE);
    for(int i = 0; i < map(p1, 127, 0, numBars, 0); i++)
      display.print(" ");
    display.setTextColor(WHITE, BLACK);
    
    for(int i = 0; i < numBars-map(p1, 127, 0, numBars, 0); i++)
      display.print(" ");
      
    display.print("] ");
    display.print(map(p1, 127, 0, 100, 1));
    display.println("%");

    display.print("[");
    display.setTextColor(BLACK, WHITE);
    for(int i = 0; i < map(p2, 127, 0, numBars, 0); i++)
      display.print(" ");
    display.setTextColor(WHITE, BLACK);
    
    for(int i = 0; i < numBars-map(p2, 127, 0, numBars, 0); i++)
      display.print(" ");
      
    display.print("] ");
    display.print(map(p2, 127, 0, 100, 1));
    display.println("%");

    display.print("[");
    display.setTextColor(BLACK, WHITE);
    for(int i = 0; i < map(p3, 127, 0, numBars, 0); i++)
      display.print(" ");
    display.setTextColor(WHITE, BLACK);
    
    for(int i = 0; i < numBars-map(p3, 127, 0, numBars, 0); i++)
      display.print(" ");
      
    display.print("] ");
    display.print(map(p3, 127, 0, 100, 1));
    display.println("%");

    display.print("[");
    display.setTextColor(BLACK, WHITE);
    for(int i = 0; i < map(p4, 127, 0, numBars, 0); i++)
      display.print(" ");
    display.setTextColor(WHITE, BLACK);
    
    for(int i = 0; i < numBars-map(p4, 127, 0, numBars, 0); i++)
      display.print(" ");
      
    display.print("] ");
    display.print(map(p4, 127, 0, 100, 1));
    display.println("%");


  }



  // Print to display
    
  display.display();
  delay(5);
  display.clearDisplay();

}

