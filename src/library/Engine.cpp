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
#include "SoundPlayer.h"
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

void GLFWCALL handle_resize_event(int width, int height) {
	assert(engine);
	glViewport(0, 0, width, height);
	engine->window_width = width;
	engine->window_height = height;
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
		glfwSleep(.01);
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
	glfwGetWindowSize(&window_width, &window_height);
	glfwSetKeyCallback(key_callback);
	glfwSetWindowCloseCallback(handle_close_event);
	glfwSetWindowSizeCallback(handle_resize_event);
	signal(SIGINT, handle_signal);
	glfwSwapInterval(swap);
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
	if(pthread_mutex_trylock(&engine_mutex)) {
		if(swap != old_swap) {
			glfwSwapInterval(swap);
			old_swap = swap;
		}
		pthread_mutex_unlock(&engine_mutex);
	}
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
	pthread_mutex_init(&engine_mutex, NULL);

	quit = false;
	has_audio = false;
	fps = -1;
	swap = old_swap = true;
	wasted = 0;

	if(!argc) {
		throw std::runtime_error("Unexpected lack of args in argv");
	}

	if(!PHYSFS_init(argv[0]) || !PHYSFS_setSaneConfig("config", app_name.c_str(), "zip", false, true)) {
		throw std::runtime_error("Failed to init PhysFS: " + std::string(PHYSFS_getLastError()));
	}

	if(alureInitDevice(NULL, NULL) == AL_FALSE) {
		printf("Failed to init audo: %s\n", alureGetErrorString());
		__SoundPlayer_audio_is_disabled = false;
	} else {
		alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
		ALSources::init();
		has_audio = true;
	}

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
	pthread_mutex_destroy(&engine_mutex);
	engine = NULL;
}
