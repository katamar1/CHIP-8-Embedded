#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_init.h>

typedef __uint16_t Address;

static bool screen[64][128];


int main(int argc, char *argv[])
{
	int i, max, c;
	int PC = 512;
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Init(SDL_INIT_VIDEO);

    SDL_CreateWindowAndRenderer("Chip-8", 0, 0, SDL_WINDOW_FULLSCREEN, &window, &renderer);
    SDL_SetRenderLogicalPresentation(renderer, 128, 64, SDL_LOGICAL_PRESENTATION_LETTERBOX);


	__uint8_t memory[4000];

	printf("Created 4KB memory.\n");
	printf("Testing bool array: %d\n", screen[4][4]);

	FILE *fp = fopen("../../2-ibm-logo.ch8", "rb");
	if (fp == NULL) {
		fprintf(stderr, "cannot open input file!\n");
		return 1;
	}

	for (i = 0, max = 4000; i < max && (c = getc(fp)) != EOF; i++) {
		/*
		printf("%02x", c);
		if (i % 16 == 15) {
			putchar('\n');
		}
		else if (i % 4 == 1) {
			putchar('\n');
		}
		*/
		memory[512 + i] = c;
	}

	/*
	printf("Done loading into memory!\n");
	fclose(fp);
	for (i = 512; i < 4000; i+= 2){
		__uint16_t opcode = (memory[i] << 8) | memory[i + 1];
		printf("%04x\n", opcode);
	}
	*/
	
	

	bool running = true;
	int tick = 0;
	bool jump = false;
	SDL_Event event;
	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_KEY_DOWN) {
				running = false;
			}
		}
		for (int i = 0; i < 12; i++) {
			tick++;
			if (tick > 200) {
				return 0;
			}
			//fetch
			__uint16_t opcode = (memory[PC] << 8) | memory[PC + 1];
			if (!jump) {
				PC += 2;
			}
			if (PC >= 1000) {
				return 0;
			}

			//decode
			__uint8_t category = (opcode & 0xF000) >> 12;
			__uint8_t X = (opcode & 0x0F00) >> 8;
			__uint8_t Y = (opcode & 0x00F0) >> 4;
			__uint8_t N = (opcode & 0x000F);
			__uint8_t NN = (opcode & 0x00FF);
			__uint16_t NNN = (opcode & 0x0FFF);
			printf("%x\n", opcode);

			//execute
			switch (category) {
				case 0x0:
					printf("Clear screen\n");
					memset(screen, 0, sizeof(screen));
					break;
				case 0x1:
					printf("Jump\n");
					PC = NNN;
					jump = true;
					break;
				case 0x6:
					printf("Set register VX\n");
					break;
				case 0x7:
					printf("Add value to register VX\n");
					break;
				case 0xA:
					printf("Set index register I\n");
					break;
				case 0xD:
					printf("Draw\n");
					break;
				default:
					printf("Unknown opcode.\n");
			}
		}

		// update display timers

		// update display

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer,255,255,255,255);
		//printf("ClearedDrawColor");
		SDL_FRect pixel = { 64.0f, 32.0f, 1.0f, 1.0f};
		SDL_RenderFillRect(renderer, &pixel);
		SDL_RenderDebugText(renderer, 10, 10, "Chip-8");
		SDL_RenderPresent(renderer);

		// delay
	}	
	return 0;
}

