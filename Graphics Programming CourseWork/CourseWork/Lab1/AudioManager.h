#pragma once
#include "Audio.h"

static Audio audioManager;

static void loadSound(const char* filename)
{
	audioManager.loadSound(filename);
}

static void playAudioNoRepeat(unsigned int Source, glm::vec3 pos)
{
	ALint state;
	alGetSourcei(Source, AL_SOURCE_STATE, &state);

	if (AL_PLAYING != state)
		audioManager.playSound(Source, pos);
}

static void playAudioNoRepeat(unsigned int Source)
{
	ALint state;
	alGetSourcei(Source, AL_SOURCE_STATE, &state);

	if (AL_PLAYING != state)
		audioManager.playSound(Source);
}