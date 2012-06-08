#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include "game.h"
#include "SoundPlayer.h"
#include <GL/glfw.h>

extern "C" {
	#include <pthread.h>
}

static SoundPlayer *player_p;
static Engine *engine_p;

void* control_loop(void*) {
	Engine &engine = *engine_p;
	SoundPlayer &player = *player_p;
	float dt = 0;
	while(!engine.should_quit()) {
		player.update_position(cos(dt)*6.0f, 0, sin(dt)*6.0f);
		dt += .1;

		if(player.player_is_stopped()) {
			printf("done playing\n");
			break;
		}
		if(engine.keyboard()[GLFW_KEY_SPACE] == GLFW_PRESS) {
			if(player.player_toggle_pause()) {
				printf("paused\n");
			} else {
				printf("resumed\n");
			}
			glfwSleep(.01);
		}
		glfwSleep(.01);


	}
	if(!player.player_is_stopped()) {
		printf("quitting\n");
	}
	engine.request_quit();
	return NULL;
}

int main(int argc, char **argv) {
	if(argc <= 1 || !strlen(argv[1])) {
		printf("Usage: %s FILENAME\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	pthread_t thread;
	void* p;
	engine_p = new Engine(argc, argv);
	player_p = new SoundPlayer(1.0f, 0, 0, 0);

	player_p->play(argv[1]);
	pthread_create(&thread, NULL, control_loop, NULL);

	while(!engine_p->should_quit()){
		engine_p->tick();
	}

	pthread_join(thread, &p);
	delete player_p;
	delete engine_p;
	return EXIT_SUCCESS;
}
