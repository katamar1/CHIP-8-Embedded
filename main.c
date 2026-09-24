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

static __uint8_t registers[16];

static __uint16_t index_register;

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
	FILE *fp = fopen("../../2-ibm-logo.ch8", "rb");

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
	bool jump = false;
	SDL_Event event;
	// int jump_count = 0;

	// This is the main function. It processes events, decodes 12 instructions, updates the timers and display, and runs 60/s.
	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_KEY_DOWN) {
				running = false;
			}
		}
		/* if (jump_count > 10) {
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
		}
		*/
		for (int m = 0; m < 12; m++) {
			// Fetch the current opcooe.
			__uint16_t opcode = (memory[PC] << 8) | memory[PC + 1];
			if (!jump) {
				PC += 2;
			}
			if (PC >= 1000) {
				return 0;
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
			switch (category) {
				case 0x0:
					printf("Clear screen\n");
					memset(screen, 0, sizeof(screen));
					break;
				case 0x1:
					printf("Jump\n");
					PC = NNN;
					jump = true;
					//jump_count++;
					break;
				case 0x6:
					printf("Set register VX\n");
					registers[X] = NN;
					break;
				case 0x7:
					printf("Add value to register VX\n");
					if (registers[X] + NN > 0xFF) {
						break;
					} else {
						registers[X] += NN; 
					}
					break;
				case 0xA:
					printf("Set index register I\n");
					index_register = NNN;
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
				default:
					printf("Unknown opcode.\n");
			}
		}

		// TODO: Add display timers and update them here.

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

