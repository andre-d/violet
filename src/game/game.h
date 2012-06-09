#pragma once

#include "Engine.h"
#include "SoundPlayer.h"

class Game : public Engine {
	void draw();
	SoundPlayer player;
	std::string filename;
	public:
		void started();
		void tick();
		Game(int argc, char** argv);
};
