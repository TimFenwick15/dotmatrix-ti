This project is to drive a 64x32 pixel dot matrix display using a TI Piccolo Launchpad F28027.

## Parts
The panel is from Pimoroni, and originally from Adafruit (I think)

A write up at http://www.rayslogic.com/propeller/Programming/AdafruitRGB/AdafruitRGB.htm pointed includes a data sheet for a similar output driver chip used on the display http://www.rayslogic.com/propeller/Programming/AdafruitRGB/MBI5026.pdf

The TI Launchpad can be found on RS.

These are connected to jumper wires, a breadboard, and a 5V 1.5A power supply I had lying around.

An image of the display working when using an Arduino: https://twitter.com/timfenwick15/status/1234520970586705920

## Project
The hope is for the display to show departure times from a Tokyo train station, to look something like:
![alt text](https://youinjapan.net/transportation/pics/japan-train-board.jpg "A Japanese departure board")

- Something like a ESP8266 polls some API
- Send data over serial/I2C to the Launchpad
- The Launchpad drives the display

A Piccolo is suited for driving a display like this because it's fast (60MHz, an Uno is 16MHz), has a lot of memory, and enough IO (we need 13 GPIO).

## Code
The code is currently a first pass at drawing some glyphs from the departure board image.

As much of the code is inlined as possible to make it fast. This has allowed the display to run with no vertical scanning visible.

The code is specific to drawing a small set of images in two colours. It would need to be generalised to allow drawing different kinds of image.

## TI
Don't yet know how to flash the controller and boot to that code on powerup. Testing using the IDE right now.

The launchpad has the following switches:
- S1-1 - Up position pulls GPIO34 up, down pulls GPIO34 down. Needs to be up to run code on power up
- S1-2 - Up position pulls TDO up, used for JTAG, needs to be up to run code on power up
- S1-3 - Up position enables JTAG, this is needed for debugging, and flashing
- S4   - Up position enables serial output (I think)

I was using:
- S1-1 - down
- S1-2 - up
- S1-3 - up
- S4   - down

Remapped LAT from GPIO34 to GPIO12 becuase 34 is used in boot mode selection. Now using:
- S1-3 all up
- S4 down

Now the code runs when the board is powered, and it can be re-flashed in the IDE.

Problem now is the code runs much more slowly when run from start up. When the board is flashed in the IDE, the code runs fine. Why is this?

Guessing the issue is the code isn't being coppied into RAM on bootup, and being run from flash.

Including InitPll in my code solved the issue. Asked about this here: https://e2e.ti.com/support/microcontrollers/c2000/f/171/p/885736/3275634

## Ghosting
This issue was resolved by switching the order the clock output is toggled in. Data is read on the rising clock edge, so clock should end the loop low and be driven high after the data is set.

There are pixels near the top left that flicker when this image is drawn.

The problem is worse the faster the code runs. Slower drawing leads to fewer errors, but visible scan lines as the display refreshes.

Swapping the glyph positions, colours, reducing the number of glyphs drawn do not affect the location of the present of ghost pixels. So far only the refresh rate of the display seems to affect error rate. Using delays in the code does not seem to reduce error rate though.

Using breakpoints, you can show the ghost particles are not requested in the software.

Swapping GPIO location does not affect mistakes.

There's some discussion of the issue: https://learn.adafruit.com/32x16-32x32-rgb-led-matrix/faq

## INO Code

INO code calls updateDisplay on interrupt
This writes the current row only
It sets the latch at the beginning to draw the row clocked out in the last interrupt, and sets OE high to disable the LEDs
It clears the latch at the end, presumably as a delay for the matrix driver, and sets OE low to enable LEDs

There's seperatre code to write to the front and back buffers, and swap them
  - Where does this get done?

  There's some automagic somewhere to manually PWM the colours

  You pulse clock to store the data?

  It uses PORTD for the data lines. This maps to digital outputs 0 - 7

