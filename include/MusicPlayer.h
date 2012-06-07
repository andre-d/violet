#pragma once

#include <string>

extern "C" {
	#include <AL/alure.h>
	#include <pthread.h>
}

class MusicPlayer {
		pthread_t thread;
		pthread_mutex_t player_mutex;
		pthread_mutex_t control_mutex;
		bool paused;
		bool stop;
	 	ALuint sourceID;            // The OpenAL sound source
	public:
	 	bool player_is_stopped();
	 	MusicPlayer();
	 	void player_stop();
	 	void play(std::string filename);
	 	void play_threaded(std::string filename);
	 	bool player_toggle_pause();
	 	virtual ~MusicPlayer();
};
