#pragma once

#include "Engine.h"
#include "SoundPlayer.h"
#include "Text.h"

class Game : public Engine {
	void draw();
	SoundPlayer player;
	std::string filename;
	Text* font;
	void gl_init();
	public:
		void started();
		void tick();
		Game(int argc, char** argv);
		~Game();
};
