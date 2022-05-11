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
	ui_init();

	pthread_t t1;

	if (pthread_create(&t1, NULL, cpu_run, NULL)) {
		printf("Err... Failed to start main Thread\n");
		return -1;
	}

	while (!ctx.die) {
		usleep(1000);
		ui_handle_event();
	}
	return 0;
}

void *cpu_run(void *p)
{
    cpu_init();
    ctx.running = true;
	ctx.paused = false;
	ctx.ticks = 0;

	while (ctx.running) {
		if (ctx.paused) {
			delay(10);
			continue;
		}
		if (!cpu_step()) {
			printf("Err... CPU Stoped\n");
			return 0;
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
	int n = cpu_cycles * 4;
	for (int i = 0; i < n; i++) {
		ctx.ticks++;
		timer_tick();
	}
}
