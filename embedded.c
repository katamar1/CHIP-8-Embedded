#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

#define START_OF_ROM 512
#define CHIP8_MEM_SIZE 4096
#define WIDTH 128
#define HEIGHT 8
#define PIN_CS 17
#define PIN_CLK 18
#define PIN_DATA 19
#define PIN_DC 20
#define PIN_RST 21
#define PIN_LED 25
#define SPI_PORT spi0
#define SPI_HZ 1000000
#define OUT true
#define IN false
typedef __uint16_t Address;

typedef __uint8_t Byte;

static bool screen[32][64];

static bool keypad[4][4];

static uint8_t frame[WIDTH * HEIGHT];

static __uint8_t registers[16];

static __uint16_t index_register;

static Address stack[16];

static int stack_current = -1;

static Byte delay_timer = 0;

static Byte sound_timer = 0;

static bool true_screen[64][128];


static const uint8_t unlock[] = {0xFD, 0x12};
static const uint8_t display_off[] = {0xAE};
static const uint8_t clock_osc[] = {0xD5, 0xA0};
static const uint8_t drive_rows[] = {0xA8, 0x3F};
static const uint8_t no_offset[] = {0xD3, 0x00};
static const uint8_t start_0[] = {0x40};
static const uint8_t mirror[] = {0xA1};
static const uint8_t reverse[] = {0xC8};
static const uint8_t row_wiring[] = {0xDA, 0x12};
static const uint8_t brightness[] = {0x81, 0xDF};
static const uint8_t pre_charge[] = {0xD9, 0x82};
static const uint8_t voltage[] = {0xDB, 0x34};
static const uint8_t disp_mem[] = {0xA4};
static const uint8_t normal[] = {0xA6};
static const uint8_t horizontal[] = {0x20, 0x00};
static const uint8_t display_on[] = {0xAF};
static const uint8_t left_right_window[] = {0x21, 0x00, 0x7F};
static const uint8_t up_down_window[] = {0x22, 0x00, 0x07};

void setup_pins() {
	uint actual_hz = spi_init(SPI_PORT, SPI_HZ);
	printf("SPI running at: %d Hz\n", actual_hz);
	spi_set_format(SPI_PORT, 8, 0, 0, SPI_MSB_FIRST);
	gpio_set_function(PIN_CLK, GPIO_FUNC_SPI);
	gpio_set_function(PIN_DATA, GPIO_FUNC_SPI);

	gpio_init(PIN_CS);
	gpio_set_dir(PIN_CS, OUT);
	gpio_init(PIN_DC);
	gpio_set_dir(PIN_DC, OUT);
	gpio_init(PIN_RST);
	gpio_set_dir(PIN_RST, OUT);
	gpio_init(PIN_LED);
	gpio_set_dir(PIN_LED, GPIO_OUT);

	gpio_put(PIN_CS, 1);
	gpio_put(PIN_DC, 0);
	gpio_put(PIN_RST, 1);
}

void write_to_screen(bool dc, const uint8_t *bytes, size_t count) {
	gpio_put(PIN_DC, dc);
	gpio_put(PIN_CS, 0);
	spi_write_blocking(SPI_PORT, bytes, count);
	gpio_put(PIN_CS, 1);
}

void command_to_screen(const uint8_t *command, size_t len) {
	write_to_screen(0, command, len);
}

void data_to_screen(const uint8_t *buffer, size_t len) {
	write_to_screen(1, buffer, len);
}

void reset_screen() {
	gpio_put(PIN_RST, 1);
	sleep_ms(1);
	gpio_put(PIN_RST, 0);
	sleep_us(10);
	gpio_put(PIN_RST, 1);
	sleep_ms(1);
}

void init_screen() {
	reset_screen();
	command_to_screen(unlock, sizeof(unlock));
	command_to_screen(display_off, sizeof(display_off));
	command_to_screen(clock_osc, sizeof(clock_osc));
	command_to_screen(drive_rows, sizeof(drive_rows));
	command_to_screen(no_offset, sizeof(no_offset));
	command_to_screen(start_0, sizeof(start_0));
	command_to_screen(mirror, sizeof(mirror));
	command_to_screen(reverse, sizeof(reverse));
	command_to_screen(row_wiring, sizeof(row_wiring));
	command_to_screen(brightness, sizeof(brightness));
	command_to_screen(pre_charge, sizeof(pre_charge));
	command_to_screen(voltage, sizeof(voltage));
	command_to_screen(disp_mem, sizeof(disp_mem));
	command_to_screen(normal, sizeof(normal));
	command_to_screen(horizontal, sizeof(horizontal));
	command_to_screen(left_right_window, sizeof(left_right_window));
	command_to_screen(up_down_window, sizeof(up_down_window));
	memset(frame, 0, sizeof(frame));
	data_to_screen(frame, sizeof(frame));
	command_to_screen(display_on, sizeof(display_on));
	sleep_ms(100);
}


int main(int argc, char *argv[])
{
	stdio_init_all();
	gpio_set_dir(PIN_LED, GPIO_OUT);
	sleep_ms(2000);
	printf("Starting\n");
	setup_pins();
	init_screen();
	printf("Init done\n");
	uint8_t turn_on[] = {0xA5};
	command_to_screen(turn_on, sizeof(turn_on));
	bool led = false;
	while (true) {
		led = !led;
		gpio_put(PIN_LED, led);
		sleep_ms(500);
	}
	int i, max, c;
	int PC = START_OF_ROM;

	// TODO: Setup display.
	

	// TODO: Setup audio.
	


	// Load the ROM into memory.
	Byte memory[CHIP8_MEM_SIZE];
	printf("Created 4KB memory.\n");
	FILE *fp = fopen("../../space_invaders.ch8", "rb");

	if (fp == NULL) {
		fprintf(stderr, "cannot open input file!\n");
		return 1;
	}

	for (i = 0, max = CHIP8_MEM_SIZE; i < max && (c = getc(fp)) != EOF; i++) {
		memory[START_OF_ROM + i] = c;
	}

	fclose(fp);

	bool running = true;
	bool blocking = false;
	bool sound_on = false;
	__uint8_t global_X = 0;
	// int jump_count = 0;

	// This is the main function. It processes events, decodes 12 instructions, updates the timers and display, and runs 60/s.
	while (running) {
				
		for (int m = 0; m < 30; m++) {
			// Fetch the current opcooe.
			// jump_count++;
			__uint16_t opcode = (memory[PC] << 8) | memory[PC + 1];
			if (blocking) {
				break;
			}
			PC += 2;
			
			// Decode the current opcode. Not all of these will be used.
			__uint8_t category = (opcode & 0xF000) >> 12;
			__uint8_t X = (opcode & 0x0F00) >> 8;
			__uint8_t Y = (opcode & 0x00F0) >> 4;
			__uint8_t N = (opcode & 0x000F);
			__uint8_t NN = (opcode & 0x00FF);
			Address NNN = (opcode & 0x0FFF);
			printf("%x\n", opcode);
			global_X = X;

			// Execute the current opcode.
			
			switch (category) {
				case 0x0:
					switch (NN) {
						case 0xEE:
							printf("Subroutine\n");
							PC = stack[stack_current];
							stack_current--;
							break;
						case 0xE0:
							printf("Clear screen\n");
							memset(screen, 0, sizeof(screen));
							break;
					}
					break;
				case 0x1:
					printf("Jump\n");
					PC = NNN;
					//jump_count++;
					break;
				case 0x2:
					printf("Subroutine\n");
					stack_current++;
					stack[stack_current] = PC;
					PC = NNN;
					break;
				case 0x3:
					printf("Skip Conditionally\n");
					if (registers[X] == NN) {
						PC += 2;
					}
					break;
				case 0x4:
					printf("Skip Conditionally\n");
					if (registers[X] != NN) {
						PC += 2;
					}
					break;
				case 0x5:
					printf("Skip Conditionally\n");
					if (registers[X] == registers[Y]) {
						PC += 2;
					}
					break;
				case 0x6:
					printf("Set register VX\n");
					registers[X] = NN;
					break;
				case 0x7:
					printf("Add value to register VX\n");
					registers[X] += NN;
					break;
				case 0x8:
					switch (N) {
						case 0x0:
							printf("Set\n");
							registers[X] = registers[Y];
							break;
						case 0x1:
							printf("Binary OR\n");
							registers[X] = registers[X] | registers[Y];
							break;
						case 0x2:
							printf("Binary AND\n");
							registers[X] = registers[X] & registers[Y];
							break;
						case 0x3:
							printf("Logical XOR\n");
							registers[X] = registers[X] ^ registers[Y];
							break;
						case 0x4:
							printf("Add\n");
							__uint8_t flag = 0;
							if (registers[X] + registers[Y] > 0XFF) {
								flag = 1;
							}
							registers[X] = registers[X] + registers[Y];
							registers[0xF] = flag;
							break;
						case 0x5:
							printf("Subtract\n");
							__uint8_t value = 0;
							if (registers[X] >= registers[Y]) {
								value = 1;
							} else {
								value = 0;
							}
							registers[X] = registers[X] - registers[Y];
							registers[0xF] = value;
							break;
						case 0x7:
							printf("Subtract\n");
							value = 0;
							if (registers[Y] >= registers[X]) {
								value = 1;
							} else {
								value = 0;
							}
							registers[X] = registers[Y] - registers[X];
							registers[0xF] = value;
							break;	
						case 0x6:
							printf("Shift\n");
							value = 0;
							if ((registers[X] & 1) == 1) {
								value = 1;
							} else {
								value = 0;
							}
							registers[X] = registers[X] >> 1;
							registers[0xF] = value;
							break;
						case 0xE:
							printf("Shift\n");
							value = 0;
							if ((registers[X] >> 7) == 1) {
								value = 1;
							} else {
								value = 0;
							}
							registers[X] = registers[X] << 1;
							registers[0xF] = value;
							break;
					}
					break;
				case 0x9:
					printf("Skip Conditionally\n");
					if (registers[X] != registers[Y]) {
						PC += 2;
					}
					break;
				case 0xA:
					printf("Set index register I\n");
					index_register = NNN;
					break;
				case 0XB:
					printf("Jump with offset\n");
					PC = NNN + registers[0];
					break;
				case 0xD:
					printf("Draw\n");
					__uint8_t x_coord = registers[X] % 64;
					__uint8_t x_coord_original = x_coord;
					__uint8_t y_coord = registers[Y] % 32;
					registers[0xF] = 0;
					// Complicated logic, but what this does is process the sprite data and draw it to the screen buffer.
					// The screen buffer of bools will be given to SDL to draw in the next step.
					for (int i = 0; i < N; i++) {
						if (y_coord > 32) {
							break;
						}
						x_coord = x_coord_original;
						Byte sprite_data = memory[index_register + i];
						__uint8_t temp_bit_helper = 0x80;
						for (int j = 7; j >= 0; j--) {
							if (x_coord > 64) {
								break;
							}
							__uint8_t pixel = (sprite_data & (temp_bit_helper >> (7 - j))) >> j;
							if (pixel == 1 && screen[y_coord][x_coord] == 1) {
								screen[y_coord][x_coord] = 0;
								registers[0xF] = 1;
							} else if (pixel == 1 && screen[y_coord][x_coord] == 0) {
								screen[y_coord][x_coord] = 1;
							}
							
							x_coord++;
						}
						y_coord++;
					}
					break;
				case 0xE:
					switch (NN) {
						case 0x9E:
							printf("Skip if key\n");
							int i = 0;
							int j = 0;
							switch (registers[X]) {
								case 0x1:
									i = 0;
									j = 0;
									break;
								case 0x2:
									i = 0;
									j = 1;
									break;
								case 0x3:
									i = 0;
									j = 2;
									break;
								case 0xC:
									i = 0;
									j = 3;
									break;
								case 0x4:
									i = 1;
									j = 0;
									break;
								case 0x5:
									i = 1;
									j = 1;
									break;
								case 0x6:
									i = 1;
									j = 2;
									break;
								case 0xD:
									i = 1;
									j = 3;
									break;
								case 0x7:
									i = 2;
									j = 0;
									break;
								case 0x8:
									i = 2;
									j = 1;
									break;
								case 0x9:
									i = 2;
									j = 2;
									break;
								case 0xE:
									i = 2;
									j = 3;
									break;
								case 0xA:
									i = 3;
									j = 0;
									break;
								case 0x0:
									i = 3;
									j = 1;
									break;
								case 0xB:
									i = 3;
									j = 2;
									break;
								case 0xF:
									i = 3;
									j = 3;
									break;
							}
							if (keypad[i][j] == 1) {
								PC += 2;
							}
							break;
						case 0xA1:
							printf("Skip if key\n");
							switch (registers[X]) {
								case 0x1:
									i = 0;
									j = 0;
									break;
								case 0x2:
									i = 0;
									j = 1;
									break;
								case 0x3:
									i = 0;
									j = 2;
									break;
								case 0xC:
									i = 0;
									j = 3;
									break;
								case 0x4:
									i = 1;
									j = 0;
									break;
								case 0x5:
									i = 1;
									j = 1;
									break;
								case 0x6:
									i = 1;
									j = 2;
									break;
								case 0xD:
									i = 1;
									j = 3;
									break;
								case 0x7:
									i = 2;
									j = 0;
									break;
								case 0x8:
									i = 2;
									j = 1;
									break;
								case 0x9:
									i = 2;
									j = 2;
									break;
								case 0xE:
									i = 2;
									j = 3;
									break;
								case 0xA:
									i = 3;
									j = 0;
									break;
								case 0x0:
									i = 3;
									j = 1;
									break;
								case 0xB:
									i = 3;
									j = 2;
									break;
								case 0xF:
									i = 3;
									j = 3;
									break;
							}
							if (keypad[i][j] == 0) {
								PC += 2;
							}
							break;
					}
					break;
				case 0xF:
					switch(NN) {
						case 0x07:
							printf("Timer\n");
							registers[X] = delay_timer;
							break;
						case 0x0A:
							printf("Get key\n");
							PC -= 2;
							blocking = true;
							break;
						case 0x15:
							printf("Timer\n");
							delay_timer = registers[X];
							break;
						case 0x18:
							printf("Timer\n");
							sound_timer = registers[X];
							break;
						case 0x65:
							printf("Load memory\n");
							for (int i = 0; i <= X; i++) {
								registers[i] = memory[index_register + i];
							}
							break;
						case 0x55:
							printf("Store memory\n");
							for (int i = 0; i <= X; i++) {
								memory[index_register + i] = registers[i];
							}
							break;
						case 0x33:
							printf("Binary-coded decimal conversion\n");
							__uint8_t divisor = 100;
							__uint8_t register_value = registers[X];
							for (int i = 0; divisor != 0; i++) {
								memory[index_register + i] = register_value / divisor;
								register_value = register_value % divisor;
								divisor = divisor / 10;
							}
							break;
						case 0x1E:
							printf("Add to index\n");
							index_register += registers[X];
							break;
					}
					break;
				default:
					printf("Unknown opcode.\n");
			}
		}
	
		if (delay_timer > 0) {
			delay_timer--;
		}
		if (sound_timer > 0) {
			if (!sound_on) {
				// TODO: resume audio stream
				sound_on = true;
			}
			sound_timer--;
		} else if (sound_on) {
			// TODO: pause audio stream
			sound_on = false;
		}

		// TODO: prep the display

		// TODO: This updates the display.
		for (int i = 0; i < 32; i++) {
			for (int j = 0; j < 64; j++) {
				if (screen[i][j] == 1) {
					// TODO: fill the pixel (2x)
					int x = j * 2;
					int y = i * 2;
					true_screen[x][y] = 1;
					true_screen[x+1][y] = 1;
					true_screen[x][y+1] = 1;
					true_screen[x+1][y+1] = 1;

				}
			}
		}
		// TODO: present the display

		// This function delays to ensure the main loop runs about 60/s.
		sleep_ms(17);
	}	
	return 0;
}

