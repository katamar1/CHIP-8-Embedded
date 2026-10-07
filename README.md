# CHIP-8-Embedded
This is a CHIP-8 emulator written in embedded C and implemented with a Raspberry Pi Pico, writing directly to the display with SPI.

# How did I go about this?
I started with the CHIP-8 emulator blogpost from Tvil: https://tobiasvl.github.io/blog/write-a-chip-8-emulator/
This covers most of the specifications of the actual computer, as well as an overview of all of the opcodes needed.

I decided to make the prototype on my laptop, rather than an embedded system. That way, I could work out all of the core emulator logic and debug it separately from the specific embedded features.

# The main emulator (main.c)
I decided to tackle this in C, using CMake for my build system and SDL for the graphics, input, and audio. I set the emulator to execute about 15 instructions a cycle (a cycle happens 60 times a second).
- I started by loading the rom into the program based on the filename.
- I then focused on getting the crucial opcodes running (such as display).
- I then used various test ROMs to take care of the standard opcodes and math opcodes.
- I then handled the delay timer and keyboard inputs - I used nanosleep for my delay.
- I then handled the sound timer and getting the beeping to work.
After all that, I could play Space Invaders! Then, it was time to port over to the RP2040.

# The embedded emulator (embedded.c)
I used the arm-toolchain and flashed the .uf2 file manually, rather than using an IDE to do so.
The wiring was fairly simple:
- The keypad had 10 pins, so I had to write a small test program to find out which 8 pins had actual data.
- The display had a few ports that I needed to wire to my RP2040.

The most difficult part of this project by far was reading the datasheet for the Adafruit OLED display. I had to execute around 15 SPI commands using the Pico-SPI functionality, and figure out how to send data to the screen in bytes.

The second part that was difficult was the keypad. It has four vertical and four horizontal wires connected to the RP2040. I had to set each row to 0 and check the value of the column wire for each of the keys in that row to see if it was pulled down, and execute a keypress accordingly.

<img width="1440" height="1920" alt="image" src="https://github.com/user-attachments/assets/0ed4272c-744a-422f-adc5-f391fd42cc6c" />

<img width="1440" height="1920" alt="image" src="https://github.com/user-attachments/assets/e52fa2ab-269e-4a85-96cc-c3a5a05abb8d" />


