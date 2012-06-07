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
	 	ALint state;                // The state of the sound source
	 	ALuint bufferID;            // The OpenAL sound buffer ID
	 	ALuint sourceID;            // The OpenAL sound source
	 	ALenum format;              // The sound data format
	 	ALsizei freq;               // The frequency of the sound data
	public:
	 	bool stop;
	 	MusicPlayer();
	 	void player_stop();
	 	void play(std::string filename);
	 	void play_threaded(std::string filename);
	 	bool player_toggle_pause();
	 	virtual ~MusicPlayer();
};
