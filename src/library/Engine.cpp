#include <cstdio>
#include <cassert>
#include <csignal>
extern "C" {
	#include <GL/glfw.h>
	#include <AL/alure.h>
	#include <pthread.h>
}
#include "Engine.h"

static Engine *engine = NULL;

/* This is here to prevent driving the GPU too rapidly without vsync */
const static int MAX_ENGINE_FPS = 240;
const static double target = (1.0/(double)MAX_ENGINE_FPS);

void handle_signal(int) {
	assert(engine);
	engine->request_quit();
}

int GLFWCALL handle_close_event() {
	assert(engine);
	return engine->request_quit() ? GL_TRUE : GL_FALSE;
}

bool Engine::quit_requested() {
	return true;
}

bool Engine::request_quit() {
	return (quit = quit_requested());
}

bool Engine::should_quit() {
	return quit;
}

void Engine::tick() {
	assert(engine);

	glfwSwapBuffers();
	glFinish();

	double time = glfwGetTime();
	double d = time - last;
	last = time;
	fps = (int)((1000.0 / (1000.0 * d)) + .5);
	double w;
	while (((w = glfwGetTime()) - last) < (target + wasted - d)) {
		pthread_yield();
	};
	wasted = w - time;
}

Engine::Engine(int, char**) {
	assert(!engine);
	quit = false;
	fps = -1;
	wasted = 0;
	alureInitDevice(NULL, NULL);
	glfwInit();
	glfwDisable(GLFW_AUTO_POLL_EVENTS);
	last = glfwGetTime();
	glfwOpenWindow(640, 480, 0, 0, 0, 0, 0, 0, GLFW_WINDOW);
	glfwSetWindowCloseCallback(handle_close_event);
	glfwSwapInterval(1);
	engine = this;
	signal(SIGINT, handle_signal);
}

Engine::~Engine() {
	glfwTerminate();
	alureShutdownDevice();
	signal(SIGINT, SIG_DFL);
	engine = NULL;
}
