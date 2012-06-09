#pragma once

#include <string>

extern "C" {
	#include <AL/alure.h>
	#include <pthread.h>
}

extern bool __SoundPlayer_audio_is_disabled;

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
		void _player_stop();
		float z;
		void init(float volume, float x, float y, float z);
	 	ALuint sourceID;            // The OpenAL sound source
	public:
	 	bool player_is_stopped();
	 	bool player_is_paused();
	 	void update_position(float x, float y, float z);
	 	void update_volume(float volume);
	 	SoundPlayer();
	 	SoundPlayer(float volume, float x, float y, float z);
	 	void player_stop();

	 	void play(std::string filename);

	 	void play_threaded(std::string filename);
	 	bool player_toggle_pause();
	 	virtual ~SoundPlayer();
};
