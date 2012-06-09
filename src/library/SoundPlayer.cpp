#include <cstdio>
#include "SoundPlayer.h"
#include "ALSources.h"

bool __SoundPlayer_audio_is_disabled = false;

void stopped_callback(void* p, ALuint) {
	((SoundPlayer*)p)->player_stop();
}

bool SoundPlayer::player_is_paused() {
	return paused;
}

bool SoundPlayer::player_is_stopped() {
	return stop;
}

void SoundPlayer::_player_stop() {
	if(!stop) {
		stop = paused = true;
		alureStopSource(sourceID, AL_FALSE);
		alureDestroyStream(stream, 1, buf);
		ALSources::release(sourceID);
		pthread_mutex_unlock(&player_mutex);
	}
}

void SoundPlayer::player_stop() {
	pthread_mutex_lock(&control_mutex);
	_player_stop();
	pthread_mutex_unlock(&control_mutex);
}

bool SoundPlayer::player_toggle_pause() {
	pthread_mutex_lock(&control_mutex);
	if(!stop) {
		if(paused) {
			alureResumeSource(sourceID);
		} else {
			alurePauseSource(sourceID);
		}
		paused = !paused;
	}
	pthread_mutex_unlock(&control_mutex);
	return paused;
}

void SoundPlayer::play(std::string filename) {
	if(__SoundPlayer_audio_is_disabled) {
		return;
	}

	pthread_mutex_lock(&control_mutex);
	_player_stop();
	pthread_mutex_lock(&player_mutex);

	stop = true;

	stream = alureCreateStreamFromFile(filename.c_str(), 19200, 2, buf);

	if(stream == AL_NONE) {
		printf("Error loading file %s: %s\n", filename.c_str(), alureGetErrorString());
	}else if(!ALSources::get(sourceID)) {
		printf("No OpenAL sources available\n");
		alureDestroyStream(stream, 1, buf);
	} else {
		stop = paused = false;
		alSourcef(sourceID, AL_GAIN, volume);
		alSource3f(sourceID, AL_POSITION, x, y, z);
		alurePlaySourceStream(sourceID, stream, 2, 0, stopped_callback, this);
	}

	pthread_mutex_unlock(&control_mutex);
}

void SoundPlayer::update_position(float x, float y, float z) {
	pthread_mutex_lock(&control_mutex);
	this->x = x;
	this->y = y;
	this->z = z;
	if(!stop) {
		alSource3f(sourceID, AL_POSITION, x, y, z);
	}
	pthread_mutex_unlock(&control_mutex);
}

void SoundPlayer::update_volume(float volume) {
	pthread_mutex_lock(&control_mutex);
	this->volume = volume;
	if(!stop) {
		alSourcef(sourceID, AL_GAIN, volume);
	}
	pthread_mutex_unlock(&control_mutex);
}

void SoundPlayer::init(float volume, float x, float y, float z) {
	stop = true;
	paused = true;
	pthread_mutex_init(&player_mutex, NULL);
	pthread_mutex_init(&control_mutex, NULL);
	update_volume(volume);
	update_position(x, y, z);
}

SoundPlayer::SoundPlayer(float volume, float x, float y, float z) {
	init(volume, x, y, z);
}

SoundPlayer::SoundPlayer() {
	init(1, 0, 0, 0);
}

SoundPlayer::~SoundPlayer() {
	player_stop();
	pthread_mutex_destroy(&player_mutex);
	pthread_mutex_destroy(&control_mutex);
}

