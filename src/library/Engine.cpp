#include <cstdio>
#include <cassert>
#include <csignal>
#include <stdexcept>

extern "C" {
	#include <GL/glfw.h>
	#include <AL/alure.h>
	#include <pthread.h>
	#include <physfs.h>
}

#include "Engine.h"
#include "ALSources.h"

/* GLFW is mostly static single-windowed */
static Engine *engine = NULL;

static int s_keyboard[GLFW_KEY_LAST] = { GLFW_RELEASE };

/* This is here to prevent driving the GPU too rapidly without vsync */
const static int MAX_ENGINE_FPS = 250;
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
	if(quit) {
		return true;
	}
	return (quit = quit_requested());
}

bool Engine::should_quit() {
	return quit;
}

void* control_loop(void* e) {
	Engine *engine = (Engine *)e;
	while(!engine->should_quit()) {
		engine->tick();
	}
	return NULL;
}

void GLFWCALL key_callback(int key, int action) {
	s_keyboard[key] = action;
}

void Engine::go() {
	pthread_t thread;
	void* p;
	glfwOpenWindow(640, 480, 0, 0, 0, 0, 0, 0, GLFW_WINDOW);
	glfwSetKeyCallback(key_callback);
	glfwSetWindowCloseCallback(handle_close_event);
	signal(SIGINT, handle_signal);
	glfwSwapInterval(0);
	started();
	pthread_create(&thread, NULL, control_loop, this);
	while(!should_quit()){
			_tick();
	}
	pthread_join(thread, &p);
}

const int* Engine::keyboard() {
	return s_keyboard;
}

void Engine::_tick() {
	assert(engine);
	alureUpdate();
	draw();
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

Engine::Engine(int argc, char** argv, std::string app_name) {
	assert(!engine);

	quit = false;
	fps = -1;
	wasted = 0;

	if(!argc) {
		throw std::runtime_error("Unexpected lack of args in argv");
	}

	if(!PHYSFS_init(argv[0]) || !PHYSFS_setSaneConfig("config", app_name.c_str(), "zip", false, true)) {
		throw std::runtime_error("Failed to init PhysFS: " + std::string(PHYSFS_getLastError()));
	}

	if(alureInitDevice(NULL, NULL) == AL_FALSE) {
		throw std::runtime_error("Failed to init ALURE (Audio): " + std::string(alureGetErrorString()));
	}

	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);

	ALSources::init();

	if(glfwInit() != GL_TRUE) {
		throw std::runtime_error("Failed to init glfw (Graphics)");
	}

	last = glfwGetTime();
	engine = this;
}

Engine::~Engine() {
	glfwTerminate();
	ALSources::deinit();
	alureShutdownDevice();
	signal(SIGINT, SIG_DFL);
	engine = NULL;
}
