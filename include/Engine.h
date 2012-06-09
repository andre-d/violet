#pragma once

class Engine {
		bool quit;
		double last;
		double wasted;
		void _tick();
	public:
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
