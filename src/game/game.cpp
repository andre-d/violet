#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <stdexcept>
#include "game.h"

extern "C" {
	#include <GL/glfw.h>
	#include <pthread.h>
}

void Game::tick() {
	if(player.player_is_stopped()) {
		printf("done playing\n");
		request_quit();
	}

	if(keyboard()[GLFW_KEY_SPACE] == GLFW_PRESS) {
		if(player.player_toggle_pause()) {
			printf("paused\n");
		} else {
			printf("resumed\n");
		}
		glfwSleep(.5);
	}
}

void Game::draw() {
	glClear(GL_COLOR_BUFFER_BIT);
	//printf("FPS: %d\n", fps);
}

Game::Game(int argc, char** argv): Engine(argc, argv, "gametest") {
	if(argc <= 1 || !strlen(argv[1])) {
		throw std::runtime_error("No filename specifed");
	}
	filename = argv[1];
	glClearColor(1.0, 1.0, 1.0, 0);
}

void Game::started() {
	player.play(filename);
}

int main(int argc, char **argv) {
	try {
		Game game(argc, argv);
		game.go();
		return EXIT_SUCCESS;
	}
	catch (std::runtime_error &e) {
		printf("There was an error: %s\n", e.what());
	}
	catch (...) {
		printf("Unknown error\n");
	}

	return EXIT_FAILURE;
}
