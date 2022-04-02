#include "include.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

static emu_context ctx;

int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("Err: gbemu <rom_file>\n");
		return -1;
	}

	if (!cart_load(argv[1])) {
		printf("Err: Neizdevas ieladet ROM failu. %s\n", argv[1]);
		return -2;
	}

	printf("Kartridza ieladeta...\n");
	
	SDL_Init(SDL_INIT_VIDEO);
	printf("SDL INIT...\n");
	TTF_Init();
	printf("TTF INIT...\n");

	cpu_init();

	ctx.running = true;
	ctx.paused = false;
	ctx.ticks = 0;

	while(ctx.running) {
		if (ctx.paused) {
			delay(10);
			continue;
		}

		if (!cpu_step()) {
			printf("Err: CPU apstajies...\n");
			return -3;
		}

		ctx.ticks++;
	}
	return 0;
}

void delay(u32 ms) {
	SDL_Delay(ms);
}

emu_context *emu_get_context() {
	return &ctx;
}

void emu_cycles(int cpu_cycles)
{
	// TODO ...
}
