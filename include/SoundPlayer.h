#pragma once

#include <string>

extern "C" {
	#include <AL/alure.h>
	#include <pthread.h>
}

class SoundPlayer {
		pthread_t thread;
		pthread_mutex_t player_mutex;
		pthread_mutex_t control_mutex;
		bool paused;
		bool stop;
		float volume;
		float x;
		float y;
		ALuint buf[2];
		alureStream * stream;
		void _player_stop(bool lock);
		float z;
	 	ALuint sourceID;            // The OpenAL sound source
	public:
	 	bool player_is_stopped();
	 	void update_position(float x, float y, float z);
	 	void update_volume(float volume);
	 	SoundPlayer(float volume, float x, float y, float z);
	 	void player_stop();
	 	void play(std::string filename);
	 	void play_threaded(std::string filename);
	 	bool player_toggle_pause();
	 	virtual ~SoundPlayer();
};
