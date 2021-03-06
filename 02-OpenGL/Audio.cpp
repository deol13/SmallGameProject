#include "Audio.h"
#include <iostream>

using namespace std;

Audio& Audio::getAudio()
{
	static Audio singleton;
	return singleton;
}

Audio::Audio()
{
	
}

Audio::~Audio()
{

}

bool Audio::init(float musicV, float soundV, float masterV, bool musicE, bool soundE, bool audioE)
{
	//init options
	applySettings(musicV, soundV, masterV, musicE, soundE, audioE);

	//load tracks
	loadFiles();

	//Init OpenAL
	device = alcOpenDevice(NULL);
	if (!device) return endWithError("no sound device");
	context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);
	if (!context) return endWithError("no sound context");

	//Listener
	listenerPos[0] = 0.0;
	listenerPos[1] = 0.0;
	listenerPos[2] = 0.0;
	ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };
	ALfloat ListenerOri[] = { 0.0, 0.0, 1.0, 0.0, 0.0, 0.0 }; //up, lookat                                                                                
	alListenerfv(AL_POSITION, listenerPos); //Set position of the listener
	alListenerfv(AL_VELOCITY, ListenerVel); //Set velocity of the listener
	alListenerfv(AL_ORIENTATION, ListenerOri);

	// create music buffers
	alGenBuffers(MUSIC_BUFFERS, musicBuffer);
	createBuffers(musicFiles, musicBuffer, MUSIC_BUFFERS);

	// create sound buffers
	alGenBuffers(SOUND_BUFFERS, soundBuffer);
	createBuffers(soundFiles, soundBuffer, SOUND_BUFFERS);

	return EXIT_SUCCESS;
}

void Audio::applySettings(float musicV, float soundV, float masterV, bool musicE, bool soundE, bool audioE)
{
	if (musicV > 1.0f) // music
		musicVolume = 1.0f;
	else
		musicVolume = musicV;

	if (soundV > 1.0f) // sound
		soundVolume = 1.0f;
	else
		soundVolume = soundV;

	if (masterV > 1.0f) // master
		masterVolume = 1.0f;
	else
		masterVolume = masterV;

	musicEnabled = musicE;
	soundEnabled = soundE;
	audioEnabled = audioE;
}

void Audio::loadFiles()
{
	//load music files
	musicFiles[0] = "../soundfiles/mainmenu.wav";
	musicFiles[1] = "../soundfiles/ingame.wav";
	musicFiles[2] = "../soundfiles/shopmusic.wav";

	soundFiles[0] = "../soundfiles/swordswing.wav";
	soundFiles[1] = "../soundfiles/spearthrust.wav";
	soundFiles[2] = "../soundFiles/Arrow.wav";

}

bool Audio::createBuffers(char** files, ALuint* buffers, int elements)
{
	for (int i = 0; i < elements; i++)
	{
		FILE *fp = NULL;
		fp = fopen(files[i], "rb"); // open audio file for reading
		if (!fp) return endWithError("Failed to open file");

		WaveHeader waveFileHeader;

		//Check that the WAVE file is OK
		fread(waveFileHeader.type, sizeof(char), 4, fp);
		if (waveFileHeader.type[0] != 'R' || waveFileHeader.type[1] != 'I'  // check RIFF
			|| waveFileHeader.type[2] != 'F' || waveFileHeader.type[3] != 'F')
			return endWithError("No RIFF");

		fread(&waveFileHeader.size, sizeof(unsigned long), 1, fp);
		fread(waveFileHeader.type, sizeof(char), 4, fp);
		if (waveFileHeader.type[0] != 'W' || waveFileHeader.type[1] != 'A' // check WAVE
			|| waveFileHeader.type[2] != 'V' || waveFileHeader.type[3] != 'E')
			return endWithError("not WAVE");

		fread(waveFileHeader.type, sizeof(char), 4, fp);
		if (waveFileHeader.type[0] != 'f' || waveFileHeader.type[1] != 'm' // check FMT
			|| waveFileHeader.type[2] != 't' || waveFileHeader.type[3] != ' ')
			return endWithError("not fmt ");

		//Now we know that the file is a acceptable WAVE file
		//Info about the WAVE data is now read and stored
		fread(&waveFileHeader.chunkSize, sizeof(unsigned long), 1, fp);
		fread(&waveFileHeader.formatType, sizeof(short), 1, fp);
		fread(&waveFileHeader.channels, sizeof(short), 1, fp);
		fread(&waveFileHeader.sampleRate, sizeof(unsigned long), 1, fp);
		fread(&waveFileHeader.avgBytesPerSec, sizeof(unsigned long), 1, fp);
		fread(&waveFileHeader.bytesPerSample, sizeof(short), 1, fp);
		fread(&waveFileHeader.bitsPerSample, sizeof(short), 1, fp);

		fread(waveFileHeader.type, sizeof(char), 4, fp);
		if (waveFileHeader.type[0] != 'd' || waveFileHeader.type[1] != 'a' // check data
			|| waveFileHeader.type[2] != 't' || waveFileHeader.type[3] != 'a')
			return endWithError("Missing DATA");

		fread(&waveFileHeader.dataSize, sizeof(unsigned long), 1, fp);

		unsigned char* buf = new unsigned char[waveFileHeader.dataSize];                            //Allocate memory for the sound data
		//number of bytes loaded.
		//Should be the same as the Data Size if OK

		//Stores the sound data
		ALuint frequency = waveFileHeader.sampleRate;                                               //The Sample Rate of the WAVE file
		ALenum format = 0;                                                            //The audio format (bits per sample, number of channels)
		ALint state;

		//Figure out the format of the WAVE file
		if (waveFileHeader.bitsPerSample == 8)
		{
			if (waveFileHeader.channels == 1)
				format = AL_FORMAT_MONO8;
			else if (waveFileHeader.channels == 2)
				format = AL_FORMAT_STEREO8;
		}
		else if (waveFileHeader.bitsPerSample == 16)
		{
			if (waveFileHeader.channels == 1)
				format = AL_FORMAT_MONO16;
			else if (waveFileHeader.channels == 2)
				format = AL_FORMAT_STEREO16;
		}
		if (!format) return endWithError("Wrong BitPerSample");                      //Not valid format

		alBufferData(buffers[i], format, buf, waveFileHeader.dataSize, frequency);                    //Store the sound data in the OpenAL buffers
		if (alGetError() != AL_NO_ERROR)
			return endWithError("Error loading ALBuffer");

		//Clean-up
		fclose(fp);
		delete[] buf;
	}

	return EXIT_SUCCESS;
}

void Audio::update(float deltaTime)
{
	// clean and remove music sources that's finished playing
	for (int i = 0; i < musicSources.size(); i++)
	{
		ALint state;
		alGetSourcei(musicSources[i].source, AL_SOURCE_STATE, &state);
		if (state != AL_PLAYING)
		{
			alDeleteSources(1, &musicSources[i].source);
			musicSources.erase(musicSources.begin() + i);
			break;
		}

	}

	// clean and remove sound sources that's finished playing
	for (int i = 0; i < soundSources.size(); i++)
	{
		ALint state;
		alGetSourcei(soundSources[i], AL_SOURCE_STATE, &state);
		if (state != AL_PLAYING)
		{
			alDeleteSources(1, &soundSources[i]);
			soundSources.erase(soundSources.begin() + i);
			break;
		}
	}
}

void Audio::playMusic(int file)
{
	if (file < MUSIC_BUFFERS && file >= 0)
	{
		if (musicSources.size() == 0)// check if no music is playing yet
		{
			MusicStruct music;
			alGenSources(1, &music.source);

			ALfloat SourcePos[] = { 0.0, 0.0, 0.0 };
			ALfloat SourceVel[] = { 0.0, 0.0, 0.0 };

			alSourcei(music.source, AL_BUFFER, musicBuffer[file]);
			alSourcef(music.source, AL_PITCH, 1.0f);
			alSourcef(music.source, AL_GAIN, masterVolume * musicVolume);
			alSourcei(music.source, AL_SOURCE_RELATIVE, AL_TRUE); // 2D sound
			alSourcefv(music.source, AL_POSITION, SourcePos);
			alSourcefv(music.source, AL_VELOCITY, SourceVel);
			alSourcei(music.source, AL_LOOPING, AL_TRUE);

			music.volume = musicVolume;
			music.state = A_PLAYING;
			alSourcePlay(music.source);
			music.track = file;

			musicSources.push_back(music);
		}
		else if (musicSources.begin()->track != file) // check so that it's not the current track
		{
			MusicStruct music;
			alGenSources(1, &music.source);

			ALfloat SourcePos[] = { 0.0, 0.0, 0.0 };
			ALfloat SourceVel[] = { 0.0, 0.0, 0.0 };

			alSourcei(music.source, AL_BUFFER, musicBuffer[file]);
			alSourcef(music.source, AL_PITCH, 1.0f);
			alSourcef(music.source, AL_GAIN, masterVolume * musicVolume);
			alSourcei(music.source, AL_SOURCE_RELATIVE, AL_TRUE); // 2D sound
			alSourcefv(music.source, AL_POSITION, SourcePos);
			alSourcefv(music.source, AL_VELOCITY, SourceVel);
			alSourcei(music.source, AL_LOOPING, AL_TRUE);

			music.volume = musicVolume;
			music.state = A_PLAYING;
			alSourcePlay(music.source);
			music.track = file;

			alSourceStop(musicSources.begin()->source);

			musicSources.insert(musicSources.begin(), music); // add new music to the start of the vector
		}

	}
	else //track is -1 stop music
	{
		if (musicSources.size() > 0)
			alSourceStop(musicSources.begin()->source);
	}
}

void Audio::playSound(int file, bool looping)
{
if (soundEnabled && audioEnabled) //sound is enabled
	if (file < SOUND_BUFFERS && soundSources.size() < SOUND_SOURCES) //check to see that there are available sound buffers
	{
		ALuint source;
		alGenSources(1, &source);

		ALfloat SourcePos[] = { 0.0, 0.0, 0.0 };
		ALfloat SourceVel[] = { 0.0, 0.0, 0.0 };

		alSourcei(source, AL_BUFFER, soundBuffer[file]);
		alSourcef(source, AL_PITCH, 1.0f);
		alSourcef(source, AL_GAIN, masterVolume * soundVolume);
		alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE); // 2D sound
		alSourcefv(source, AL_POSITION, SourcePos);
		alSourcefv(source, AL_VELOCITY, SourceVel);
		alSourcei(source, AL_LOOPING, looping);

		alSourcePlay(source);

		soundSources.push_back(source);
	}
}

void Audio::playSoundAtPos(int file, glm::vec3 pos, float distance, bool looping)
{
if (soundEnabled && audioEnabled) //sound is enabled
	if (file < SOUND_BUFFERS && soundSources.size() < SOUND_SOURCES) //check to see that there are available sound buffers
	{
		ALuint source;
		alGenSources(1, &source);

		ALfloat SourcePos[] = { pos.x, pos.y, pos.z };
		ALfloat SourceVel[] = { 0.0, 0.0, 0.0 };

		alSourcei(source, AL_BUFFER, soundBuffer[file]);
		alSourcef(source, AL_PITCH, 1.0f);
		alSourcef(source, AL_GAIN, masterVolume * soundVolume);
		alSourcei(source, AL_REFERENCE_DISTANCE, distance);
		alSourcefv(source, AL_POSITION, SourcePos);
		alSourcefv(source, AL_VELOCITY, SourceVel);
		alSourcei(source, AL_LOOPING, looping);

		alSourcePlay(source);

		soundSources.push_back(source);
	}
}

void Audio::updateListener(glm::vec3 pos)
{
	// convert from float to ALfloat
	listenerPos[0] = pos.x;
	listenerPos[1] = pos.y;
	listenerPos[2] = pos.z;

	// update pos
	alListenerfv(AL_POSITION, listenerPos);
}

ALfloat* Audio::getListenerPos()
{
	return listenerPos;
}

void Audio::shutdown()
{
	// music
	musicSources.clear();
	alDeleteBuffers(MUSIC_BUFFERS, musicBuffer);

	// sounds
	soundSources.clear();
	alDeleteBuffers(SOUND_BUFFERS, soundBuffer);

	// device and devcon
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);
}

int Audio::endWithError(char* msg, int error)
{
	// display error message in console
	cout << msg << "\n";
	system("PAUSE");
	return error;
}
