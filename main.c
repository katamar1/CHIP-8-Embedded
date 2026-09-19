#include <stdio.h>

int main(void)
{
	printf("Welcome to the Embedded CHIP-8 Emulator!\n");

	// Memory

	// CHIP-8 will have 4 KB of RAM - more than enough on the Pico board.
	// The index register and PC can both only store 16 bits.
	// In order to be compatible with older games, the interpreter should start at 0x200.
	// Before this address, we'll need to make the font.


	// Font

	// Store the font at 0x050 to 0x09F, following popular convention.
	// Will want to define macros for the memory locations of the characters.
	// These will be drawn just like regular sprites.

	// Display

	// CHIP-8 is natively 64x32, but I will be using a 128x64 display (must be scaled up).
	// The screen is made up of bools. Can I write to each pixel on the display natively?
	// Only modify the display when an instruction is executed to change something on the display.
	// Bits in the sprite will logical XOR with the existing ones - will cause flickering.

	// Stack

	// CHIP-8 has a stack, and only uses it to call from subroutines.
	// It will only store 12 bit addresses.
	// Limit it to 22 of these addresses, closest to the original limit while stile being whole.

	// Timers

	// There are two timer registers: sound and delay.
	// As long as they are above 0, they should be deincremented by 1 sixty times a second.
	// This is where FreeRTOS comes in handy!
	// If the sound register is above 0, the computer should "beep" (a speaker).

	// Keypad

	// We'll have a 4x4 matrix of buttons that matches it pretty closely design wise.
	// For a placeholder: we'll use a matrix of the following keystroke signals:
	// 1 2 3 4
	// Q W E R
	// A S D F
	// Z X C V

	// Fetch/decode/execute loop

	// The main task. It will run in an infinite loop, and:
	// Fetch the instruction from the PC
	// Decode the instruction to find out what to do
	// Execute the instruction and do what it tells you
	// Will likely want to do around 700 instructions a second.

	// Fetch

	// Read the two bytes the PC is pointing at.
	// Immediately increment the PC by two.

	// Decode

	// Use switch statements, mask off the first hex number in the instruction.
	// Have one case per number, use break; so it doesn't start falling through.
	// X: the second nibble. Used to look up 1 of 16 registers.
	// Y: the third nibble. Same case as above.
	// N: the fourth nibble. A four bit number.
	// NN: second byte. 8-bit immediate number.
	// NNN: second, third, fourth nibbles. 12-bit memory address.
	// Extract these before decoding to avoid redundancy.
	// Use macros to make this easier!

	// Executa

	// Start with: clear screen, jump, set register, add value to reg, set index reg, display.
	// Can test these with the IBM logo program.
	// 00E0: Clear Screen
	// 1NNN: Jump
	// 00EE and 2NNN: Subroutines
	// 3XNN, 4XNN, 5XY0, 0XY0: Skip Conditionally
	// 6XNN: Set
	// 7XNN: Add
	// 8XY0: Set
	// 8XY1: Binary OR
	// 8XY2: Binary AND
	// 8XY3: Binary SOR
	// 8XY4: Add
	// 8XY5 and 8XY7: Subtract
	// 8XY6 and 8XYE: Shift
	// ANNN: Set Index
	// BNNN: Jump with offset
	// CXNN: Random
	// DXYN: Display
	// EX9E and EXA1: Skip if key
	// FX07,FX15 and FX18: Timers
	// FX1E: Add to index
	// FX0A: Get Key
	// FX29: Font Character
	// FX33: Binary Coded decimal conversion
	// FX55 and FX65: Store and load memory
	return 0;
}

