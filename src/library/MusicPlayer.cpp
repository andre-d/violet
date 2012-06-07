#include "MusicPlayer.h"
#include <cstdio>

struct PlayerData {
	std::string filename;
	MusicPlayer* player;
};

void stopped_callback(void* p, ALuint) {
	((MusicPlayer*)p)->player_stop();
}

void* start_thread_play(void *p) {
	PlayerData* pdata = (PlayerData*) p;
	pdata->player->play_threaded(pdata->filename);
	delete pdata;
	return NULL;
}

void MusicPlayer::play(std::string filename) {
	PlayerData *p = new PlayerData;
	p->player = this;
	p->filename = filename;
	pthread_mutex_lock(&control_mutex);
	stop = true;
	pthread_mutex_lock(&player_mutex);
	pthread_create(&thread, NULL, start_thread_play, (void*) p);
	stop = false;
}

bool MusicPlayer::player_is_stopped() {
	return stop;
}

void MusicPlayer::player_stop() {
	pthread_mutex_lock(&control_mutex);
	stop = true;
	pthread_mutex_unlock(&control_mutex);
}

bool MusicPlayer::player_toggle_pause() {
	pthread_mutex_lock(&control_mutex);
	if(stop) {
		return true;
	}
	if(paused) {
		alureResumeSource(sourceID);
	} else {
		alurePauseSource(sourceID);
	}
	paused = !paused;
	pthread_mutex_unlock(&control_mutex);
	return paused;
}

void MusicPlayer::play_threaded(std::string filename) {
	ALuint buf[2];
	alureStream * stream = alureCreateStreamFromFile(filename.c_str(), 19200, 2, buf);

	if(stream == AL_NONE) {
		printf("Error loading file %s: %s\n", filename.c_str(), alureGetErrorString());
		stop = true;
		pthread_mutex_unlock(&control_mutex);
		pthread_mutex_unlock(&player_mutex);
		return;
	}
	paused = false;

	alurePlaySourceStream(sourceID, stream, 2, 0, stopped_callback, this);
	pthread_mutex_unlock(&control_mutex);

	do {
		alureSleep(.1);
		alureUpdate();
	} while (!stop);

	alureStopSource(sourceID, AL_FALSE);
	alureDestroyStream(stream, 1, buf);
	pthread_mutex_unlock(&player_mutex);
}

MusicPlayer::MusicPlayer() {
	stop = true;
	pthread_mutex_init(&player_mutex, NULL);
	pthread_mutex_init(&control_mutex, NULL);
	alGenSources(1, &(sourceID));
	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	alSource3f(sourceID, AL_POSITION, 0.0f, 0.0f, 0.0f);
}

MusicPlayer::~MusicPlayer() {
	void* p;
	stop = true;
	pthread_join(thread, &p);
	pthread_mutex_destroy(&player_mutex);
	pthread_mutex_destroy(&control_mutex);
	alDeleteSources(1, &sourceID);
}

