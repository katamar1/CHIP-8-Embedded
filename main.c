#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_init.h>
#include <time.h>

#define START_OF_ROM 512
#define CHIP8_MEM_SIZE 4000
typedef __uint16_t Address;

typedef __uint8_t Byte;

static bool screen[32][64];

static bool keypad[4][4];

static __uint8_t registers[16];

static __uint16_t index_register;

static Address stack[16];

static int stack_current = -1;

static Byte delay_timer = 0;

// This function exists to be called once within the main function, to make sure that it executes 60 times per second.
void delay(long milliseconds) {
	struct timespec req;
	req.tv_sec = milliseconds / 1000;
	req.tv_nsec = (milliseconds % 1000) * 1000000L;
	nanosleep(&req, NULL);
}

int main(int argc, char *argv[])
{
	int i, max, c;
	int PC = START_OF_ROM;

	// Setup for SDL.
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer("Chip-8", 0, 0, SDL_WINDOW_FULLSCREEN, &window, &renderer);
    SDL_SetRenderLogicalPresentation(renderer, 64, 32, SDL_LOGICAL_PRESENTATION_LETTERBOX);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer,255,255,255,255);



	// Load the ROM into memory.
	Byte memory[CHIP8_MEM_SIZE];
	printf("Created 4KB memory.\n");
	FILE *fp = fopen("../../5-quirks.ch8", "rb");

	if (fp == NULL) {
		fprintf(stderr, "cannot open input file!\n");
		return 1;
	}

	for (i = 0, max = CHIP8_MEM_SIZE; i < max && (c = getc(fp)) != EOF; i++) {
		/*
		printf("%02x", c);
		if (i % 16 == 15) {
			putchar('\n');
		}
		else if (i % 4 == 1) {
			putchar('\n');
		}
		*/
		memory[START_OF_ROM + i] = c;
	}

	fclose(fp);

	/*
	printf("Done loading into memory!\n");
	fclose(fp);
	for (i = 512; i < 4000; i+= 2){
		__uint16_t opcode = (memory[i] << 8) | memory[i + 1];
		printf("%04x\n", opcode);
	}
	*/

	bool running = true;
	bool blocking = false;
	SDL_Event event;
	// int jump_count = 0;

	// This is the main function. It processes events, decodes 12 instructions, updates the timers and display, and runs 60/s.
	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_KEY_DOWN) {
				for(int i = 0; i < 1000; i++) {
					printf("Name of key: %s\n", SDL_GetKeyName(event.key.key));
				}
				if (event.key.scancode == SDL_SCANCODE_1) {
					keypad[0][0] = 1;
					for (int i = 0; i < 1000; i++) {
						printf("1 KEY PRESSED!\n");
					}
				} else {
					running = false;
				}
			}
			else if (event.type == SDL_EVENT_KEY_UP) {
				if (blocking) {
					blocking = false;
					PC += 2;
				}
				if (event.key.scancode == SDL_SCANCODE_1) {
					keypad[0][0] = 0;
					for (int i = 0; i < 1000; i++) {
						printf("1 KEY LIFTED!\n");
					}
				} else {
					running = false;
				}
			}
		}

		/*
		
		if (jump_count > 40) {
			for (int l = 0; l < 32; l++) {
				for (int k = 0; k < 64; k++) {
					if (screen[l][k] == 0) {
						printf(". ");
					} else {
						printf("%u ", screen[l][k]);
					}
				}
				printf("\n");
			}
			running = false;
			break;
		}
		*/
		
		for (int m = 0; m < 12; m++) {
			// Fetch the current opcooe.
			// jump_count++;
			__uint16_t opcode = (memory[PC] << 8) | memory[PC + 1];
			if (!blocking) {
				PC += 2;
			}
			
			// Decode the current opcode. Not all of these will be used.
			__uint8_t category = (opcode & 0xF000) >> 12;
			__uint8_t X = (opcode & 0x0F00) >> 8;
			__uint8_t Y = (opcode & 0x00F0) >> 4;
			__uint8_t N = (opcode & 0x000F);
			__uint8_t NN = (opcode & 0x00FF);
			Address NNN = (opcode & 0x0FFF);
			printf("%x\n", opcode);

			// Execute the current opcode.
			
			// TODO: Add subroutine opcodes.
			// TODO: Add logical and arithmetic instruction opcodes.
			// TODO: Add jump with index opcode.
			// TODO: Add random opcode.
			// TODO: Implement keyboard and skip if key opcodesx.
			// TODO: Add timer opcodes.
			// TODO: Add add to index opcode.
			// TODO: Add get key opcode.
			// TODO: Add the fonts in memory and implement font character opcode.
			// TODO: Add binary-coded decimal conversion opcode.
			// TODO: Add store and load memory opcodes.
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
							break;
						case 0x15:
							printf("Timer\n");
							delay_timer = registers[X];
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
							/*if (registers[X] / 100 > 0) {
								divisor = 100;
							} else if (registers[X] / 10 > 0) {
								divisor = 10;
							} else {
								divisor = 1;
							}
							*/
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

		// TODO: Add display timers and update them here.
	
		if (delay_timer > 0) {
			delay_timer--;
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

		// This updates the display with SDL.
		for (int i = 0; i < 32; i++) {
			for (int j = 0; j < 64; j++) {
				if (screen[i][j] == 1) {
					SDL_FRect pixel = {j, i, 1.0f, 1.0f};
					SDL_RenderFillRect(renderer, &pixel);
				}
			}
		}
		SDL_RenderPresent(renderer);

		// This function delays to ensure the main loop runs about 60/s.
		delay(17);
	}	
	return 0;
}

