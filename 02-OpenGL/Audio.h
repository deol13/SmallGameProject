#ifndef AUDIO_H
#define AUDIO_H

#include <conio.h>
#include <stdlib.h>
#include <vector>

#include <glm\glm.hpp>
#include <AL\alut.h>
#include <AL\al.h>

//states
#define A_PLAYING 0
#define A_PAUSED 1
#define A_FADEIN 2
#define A_FADEOUT 3
#define FADEINTIME 0.15f
#define FADEOUTTIME 0.4f
#define MUSIC_BUFFERS 3 // one buffer for each file
#define SOUND_BUFFERS 3 // one buffer for each file
#define SOUND_SOURCES 32 // maximum number of simultanious sounds

using namespace std;

class Audio
{
private:
	struct WaveHeader
	{
		char type[4];
		unsigned long size, chunkSize;
		short formatType, channels;
		unsigned long sampleRate, avgBytesPerSec;
		short bytesPerSample, bitsPerSample;
		unsigned long dataSize;
	};

	struct MusicStruct
	{
		int state;
		int track;
		ALuint source;
		ALfloat volume;
	};


public:
	static Audio& getAudio();
	Audio();
	~Audio();
	bool init(float musicV, float soundV, float masterV, bool musicE, bool soundE, bool audioE);
	void applySettings(float musicV, float soundV, float masterV, bool musicE, bool soundE, bool audioE);
	void update(float deltaTime);
	void playMusic(int file);
	void playSound(int file, bool looping);
	void playSoundAtPos(int file, glm::vec3 pos, float distance, bool looping);
	void updateListener(glm::vec3 pos);
	ALfloat* getListenerPos();
	void shutdown();

private:
	void loadFiles();
	bool createBuffers(char** files, ALuint* buffers, int elements);
	int endWithError(char* msg, int error = 0);

private:
	
	//OpenAL device and context
	ALCdevice *device;
	ALCcontext *context;

	//effects extention
	

	//listener
	ALfloat listenerPos[3];

	//files
	char* musicFiles[MUSIC_BUFFERS];
	char* soundFiles[SOUND_BUFFERS];

	//buffers
	ALuint musicBuffer[MUSIC_BUFFERS];
	ALuint soundBuffer[SOUND_BUFFERS];

	// audio source lists
	vector<ALuint> soundSources;
	vector<MusicStruct> musicSources;

	//settings
	float musicVolume, soundVolume, masterVolume;
	bool musicEnabled, soundEnabled, audioEnabled;
};

#endif