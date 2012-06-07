#pragma once

class Engine {
		bool quit;
		double last;
		double wasted;
		int fps;
	public:
		const int* keyboard();
		pthread_mutex_t mutex;
		bool should_quit();
		bool quit_requested();

		int get_fps();

		void tick();

		Engine(int argc, char **argv);

		virtual bool request_quit();
		virtual ~Engine();
};
