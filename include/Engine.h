#pragma once

class Engine {
		bool quit;
		double last;
		double wasted;
		void _tick();
	protected:
		pthread_mutex_t engine_mutex;
		bool swap;
		bool old_swap;
	public:
		int window_width;
		int window_height;

		bool has_audio;

		int fps;

		const int* keyboard();
		bool should_quit();
		virtual bool quit_requested();

		int get_fps();

		void go();

		virtual void started()=0;

		virtual void tick()=0;

		virtual void draw() = 0;

		Engine(int argc, char **argv, std::string app_name);

		bool request_quit();
		virtual ~Engine();
};
