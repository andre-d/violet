#pragma once

extern "C" {
	#include <AL/alure.h>
}

class ALSources {
	public:
		static bool get(ALuint &dest);
		static void release(ALuint source);
		static void init();
		static void deinit();
};
