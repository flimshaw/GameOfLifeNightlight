# Game of Life Nightlight
Source code for my Arduino/Trinket based game of life nightlight

## Parts List:
1. Apple USB Cube charger
2. [Male USB Connector](https://www.digikey.com/product-detail/en/A-USBPA-2/AE10637-ND/2170553)
3. [Adafruit i2c LED Matrix module](https://www.adafruit.com/products/871)
4. [Adafruit Trinket](https://www.adafruit.com/product/1501)
5. 2x 1/4" 2-56 screws (for LED Matrix)
6. 4x 1/8" 4-40 screws (for Trinket)

## Construction
I tried lots of different sequences to build these things, I've found the procedure
below to work best, but I'm sure there are better ways to do it.  Snapping the USB
charger into place is the very last step, as the spacing is pretty tight and you'll
need all the room you can get while soldering.

1. Mount the trinket and LED matrix in the enclosure
2. Cut, strip, and tin 6 wires 3 3/8" in length
3. Tin all 4 pads on the display module, and the BAT, GND, #0 and #2 pads on the Trinket
4. Solder two wires each to the V+ and V- pads on the USB connector. Use a
   multimeter to make sure which is which.
5. Solder the V+ wires to the BAT and VCC pads on the Trinket and LED Matrix.
6. Solder the V- wires to the GND pad on both modules.
7. Solder a wire between matrix SDA pin and Trinket pin #0
8. Solder a wire between matrix SCL pin and Trinket pin #2
9. Plug in a microUSB wire to the Trinket through the side port, and program the firmware
10. Test the module by plugging the USB connector into the Cube charger
11. If everything works, carefully snap the USB charger into place in the enclosure

I have built two dozen of these through the methods above, so they should work. If you
assemble everything and you're having problems, test your solder connections and
make sure your USB connector is properly seated and outputting power.

If the USB plug isn't snapping in securely, there is a notch for a nytie to add
some additional strength to the mount.
