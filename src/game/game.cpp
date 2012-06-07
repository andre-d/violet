#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "game.h"
#include "MusicPlayer.h"
#include <GL/glfw.h>

extern "C" {
	#include <pthread.h>
}

static MusicPlayer *player_p;
static Engine *engine_p;

void* control_loop(void*) {
	Engine &engine = *engine_p;
	MusicPlayer &player = *player_p;
	while(!engine.should_quit()) {
		if(player_p->stop) {
			printf("done playing\n");
			engine.request_quit();
		}
		if(glfwGetKey(GLFW_KEY_SPACE) == GLFW_PRESS) {
			if(player.player_toggle_pause()) {
				printf("paused\n");
			} else {
				printf("resumed\n");
			}
			glfwSleep(.1);
		}
		glfwSleep(.1);
	}
	if(!player.stop) {
		printf("quitting\n");
	}
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
	player_p = new MusicPlayer();

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
