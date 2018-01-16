# ArduinoMidiController
These go with the MIDI controllers I've built.

## Code Contents:

* __MidiControllerTest1__
	* Initial test with the MIDI protocol designed with my first, smaller controller in mind:

* __MidiController1__
	* Working version of a controller with pitch knob, pitch bend, velocity knob, channel knob and one general controller. Has a single button to send a note.

* __MidiController2__
	* Multi-mode controller. One mode is exactly like MidiController1, the other make the knobs all general controllers and uses two other buttons to change the output note.
	* This and MidiController1 were designed with my larger controller in mind:


## Code Dependencies:

* For now, these use the AdaFruit GFX and AdaFruit SSD1306 libraries to display output. You can find them here:
	* https://github.com/adafruit/Adafruit-GFX-Library
	* https://github.com/adafruit/Adafruit_SSD1306

## Parts Used:

#### Larger Controller
* 1x Arduino Uno
* 1x SSD1306 SPI OLED Display
* 5x Potentiometers
* 4x Small buttons
* 4x 10K Ohm resistors
* 1x RGB LED
* 1x [Sanoxy USB MIDI Adapter](https://www.amazon.com/gp/product/B001HPL8B2)

Feel free to use or adapt this code to you heart's content.
Accompanying circuit diagrams through Fritzing still to come...
