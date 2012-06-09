#include <cstdlib>
#include <cstdio>
#include <sstream>
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
		request_quit();
	}

	if(keyboard()[GLFW_KEY_SPACE] == GLFW_PRESS) {
		player.player_toggle_pause();
	}else if(keyboard()[(int)'V'] == GLFW_PRESS) {
		pthread_mutex_lock(&engine_mutex);
		swap = !swap;
		pthread_mutex_unlock(&engine_mutex);
	}else{
		return;
	}
	glfwSleep(.5);
}

void Game::draw() {
	std::ostringstream convert;
	gl_init();
	glClear(GL_COLOR_BUFFER_BIT);
	convert << fps << " FPS";
	glColor3f(0, 0, 0);
	glRasterPos2d(0,15);
	convert << " - Paused (Space): " << player.player_is_paused();
	convert << " - VSync (V): " << swap;
	font->draw(convert.str(), 15);
}

Game::Game(int argc, char** argv): Engine(argc, argv, "gametest") {
	if(argc <= 1 || !strlen(argv[1])) {
		throw std::runtime_error("No filename specifed");
	}
	filename = argv[1];
	font = new Text("bin/fonts/font.ttf");
}

Game::~Game() {
	delete font;
}

void Game::gl_init() {
	glClearColor(1.0, 1.0, 1.0, 0);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	glOrtho(0, window_width, window_height, 0, 0, 1);
	glMatrixMode (GL_MODELVIEW);
	glDisable(GL_DEPTH_TEST);
}

void Game::started() {
	player.play(filename);
	player.player_toggle_pause();
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
