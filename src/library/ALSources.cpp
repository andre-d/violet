#include <set>
#include <cstdio>

#include "ALSources.h"

static const ALuint MAX_ENGINE_SOURCES = 128;
static ALuint sources[MAX_ENGINE_SOURCES] = { 0 };
static ALuint NUM_SOURCES = 0;

static std::set<ALuint> unavailable;

bool ALSources::get(ALuint &dest) {
	for(ALuint i=0;i<NUM_SOURCES;i++) {
		if(!unavailable.count(sources[i])) {
			unavailable.insert(sources[i]);
			dest = sources[i];
			return true;
		}
	}
	return false;
}

void ALSources::release(ALuint source) {
	return;
	unavailable.erase(source);
}

void ALSources::init() {
	bool zerocheck;

	for(ALuint i=0;i<MAX_ENGINE_SOURCES;i++) {
		zerocheck = alIsSource(0); // Apparently 0 can be a valid source name
		alGenSources(1, sources + i);
		if((zerocheck && !sources[i]) || (!alIsSource(sources[i]))) {
			alGetError();
			break;
		}
		NUM_SOURCES++;
	}


}

void ALSources::deinit() {
	for(;NUM_SOURCES>0;NUM_SOURCES--) {
		alDeleteSources(1, sources + NUM_SOURCES - 1);
		sources[NUM_SOURCES - 1] = 0;
	}
}
