/*
8 bit signed AUDIO_S8 / signed 8-bit samples

22050 samples per second / 1 channel

- First, generate two seconds of audio.
- Every second after, generate another second of audio & queue it.

- queue (push) vs callback pull method?

Square wave initially

later:
- beat generator
- decouple song generation from sound generation

*/

#include <iostream>
#include <SDL.h>

#define samplespersec 22050
#define numchannels 1
#define samplefmt AUDIO_U8
#define samplesz 4096;


#define B4Freq 493.90f
#define A4Freq 440.0f
#define G4Freq 392.0f 
#define F4Freq 349.22f
#define E4Freq 329.6f
#define D4Freq 293.67f
#define C4Freq 261.623f


#define min(a,b) (((a) < (b)) ? (a) : (b)) // min: Choose smaller of two scalars.
#define max(a,b) (((a) > (b)) ? (a) : (b)) // max: Choose greater of two scalars.
#define Clamp(clampee, minVal,maxVal)      min(max(clampee,minVal),maxVal)         // clamp: Clamp value into set range.

void PlayWAV();
void SquareWave(int freq, int length);
void SineWave(int freq, int length);
void SawtoothWave(int freq, int length);
void PlayNoise(int length, bool lowPitch);
void SetupAudio();

SDL_AudioSpec wavSpec;
char menuInput;


const float twelthRootOf2 = powf(2.0f, 1.0f / 12.0f);

/*

Perfect fifth is 3:2.
Maj is wwhwwwh
formula for freq is:

fn = f0 * (a)n
a = twelthRootOf2
n = num of semitones away from root freq (f0)

*/


static struct songSettings
{
	int BPM = 120;
	int beatsToBar = 4;
	int temporalResolution = 32;
	float keyFreq = 261.625F; // Middle C
	SDL_AudioSpec audSpecWant, audSpecHave;

	SDL_AudioDeviceID audDevice;

} songSettings;



struct majorKey
{
	float keyFreq = 440.0f;

	float firstInterval = 0.0f;
	float secondInterval = 0.0f;
	float thirdInterval = 0.0f;
	float fourthInterval = 0.0f;
	float fifthInterval = 0.0f;
	float sixthInterval = 0.0f;
	float seventhInterval = 0.0f;
	float octave = 0.0f;

	majorKey(float freq)
	{
		this->keyFreq = freq;

		firstInterval = this->keyFreq;
		secondInterval = this->keyFreq * powf(twelthRootOf2, 2.0f);
		thirdInterval = this->keyFreq * powf(twelthRootOf2, 4.0f);
		fourthInterval = this->keyFreq * powf(twelthRootOf2, 6.0f);
		fifthInterval = this->keyFreq * powf(twelthRootOf2, 7.0f); //keyFreq / 3 * 2;
		sixthInterval = this->keyFreq * powf(twelthRootOf2, 9.0f);
		seventhInterval = this->keyFreq * powf(twelthRootOf2, 11.0f);
		octave = this->keyFreq * powf(twelthRootOf2, 12.0f);
	}
};

struct minorKey
{
	float keyFreq = 440.0f;

	float firstInterval = 0.0f;
	float secondInterval = 0.0f;
	float thirdInterval = 0.0f;
	float fourthInterval = 0.0f;
	float fifthInterval = 0.0f;
	float sixthInterval = 0.0f;
	float seventhInterval = 0.0f;
	float octave = 0.0f;

	minorKey(float freq)
	{
		this->keyFreq = freq;

		firstInterval = this->keyFreq;
		secondInterval = this->keyFreq * powf(twelthRootOf2, 2.0f);
		thirdInterval = this->keyFreq * powf(twelthRootOf2, 3.0f);
		fourthInterval = this->keyFreq * powf(twelthRootOf2, 5.0f);
		fifthInterval = this->keyFreq * powf(twelthRootOf2, 7.0f); //keyFreq / 3 * 2;
		sixthInterval = this->keyFreq * powf(twelthRootOf2, 8.0f);
		seventhInterval = this->keyFreq * powf(twelthRootOf2, 10.0f);
		octave = this->keyFreq * powf(twelthRootOf2, 12.0f);
	}
};


int main(int argc, char * argv[])
{
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		std::cout << "Could not initialise SDL";
	}
	else
	{
		std::cout << "SDL initialised!!\n"
		"Press w to play wav\n"
		"Press g to generate square wave\n"
		"Press s to generate waves\n"
		"Press r to generate random song\n"
		"Press q to quit";
	}


	// Setup audio device & audio spec
	SetupAudio();

	menuInput = std::cin.get();

	if (menuInput == 'w')
		PlayWAV();
	else if (menuInput == 'g')
	{
		SquareWave(440.0f, 1000);
		_sleep(2000);
	}
	else if (menuInput == 's')
	{
		SineWave(440.0f, 1000);
		_sleep(2000);
		SquareWave(440.0f, 500);
		_sleep(2000);
		SawtoothWave(440.0f, 1000);
		_sleep(2000);
		PlayNoise(1000, false);
		_sleep(1000);
		PlayNoise(1000, true);
		_sleep(1000);

	}
	else if (menuInput == 'r')
	{
		std::cout << "\n Generating a song with a BPM of " << songSettings.BPM << " and " << songSettings.beatsToBar << " beats to a bar.  Song will be generated in the key of C4";
		majorKey CMaj(C4Freq); // Middle C);

		std::cout << "\n Notes in key: "
			<< CMaj.firstInterval << " "
			<< CMaj.secondInterval << " "
			<< CMaj.thirdInterval << " "
			<< CMaj.fifthInterval << " "
			<< CMaj.seventhInterval << " "
			<< CMaj.octave << "\n";

		for (int c = 0; c < 3; c++)
		{
			SquareWave(CMaj.firstInterval, 75);
			_sleep(50);
			SquareWave(CMaj.thirdInterval, 75);
			_sleep(50);
			SquareWave(CMaj.fifthInterval, 75);
			_sleep(50);
			SquareWave(CMaj.octave, 75);
			_sleep(50);
			SquareWave(CMaj.fifthInterval, 75);
			_sleep(50);
			SquareWave(CMaj.thirdInterval, 75);
			_sleep(50);
			SquareWave(CMaj.firstInterval, 75);
			_sleep(50);
			SquareWave(CMaj.fourthInterval, 75);
			_sleep(50);
			SquareWave(CMaj.fifthInterval, 75);
			_sleep(50);
			SquareWave(CMaj.octave, 75);
			_sleep(50);
			SquareWave(CMaj.fifthInterval, 75);
			_sleep(50);
			SquareWave(CMaj.fourthInterval, 75);
			_sleep(50);
		}


		std::cin.get();
	}
	else if (menuInput == 'q')
	{
		SDL_Quit();
		exit(0);
	}
		

	std::cin.get();

	return 0;
}

void SetupAudio()
{
	SDL_memset(&songSettings.audSpecWant, 0, sizeof(songSettings.audSpecWant));

	songSettings.audSpecWant.freq = samplespersec;
	songSettings.audSpecWant.format = samplefmt;
	songSettings.audSpecWant.channels = numchannels;
	songSettings.audSpecWant.samples = 4096;
	songSettings.audSpecWant.callback = NULL;

	songSettings.audDevice = SDL_OpenAudioDevice(NULL, 0, &songSettings.audSpecWant, &songSettings.audSpecHave, 0);

}

void PlayWAV()
{

	Uint32 wavLength;
	Uint8 *wavBuffer;

	std::cout << "Playing WAV";

	SDL_LoadWAV("stage1.wav", &songSettings.audSpecWant, &wavBuffer, &wavLength);

	int success = SDL_QueueAudio(songSettings.audDevice, wavBuffer, wavLength);

	if (success != 0)
		std::cout << "\nError outputting to device!!\n";

	// Play audio finally after unpausing device.

	SDL_PauseAudioDevice(songSettings.audDevice, 0);

}

void SquareWave(int freq, int length) // length is specified in ms
{
	// Generate square wave of A4 with enough data to fill one second of an audio buffer.
	std::cout << "Generating square\n";

	Uint32 squareWaveLength;
	Uint8 *squareWaveBuffer;
	

	squareWaveLength = (samplespersec / 1000) * length;
	squareWaveBuffer = new Uint8[squareWaveLength];

	int changeSignEveryXCycles = samplespersec / (freq * 2); // 440 = A4.  Value is doubled as the entire square wave has to be 440 hz.
	bool writeHigh = false;


	for (int c = 0; c < squareWaveLength; c++)
	{
		if (c % changeSignEveryXCycles == 0)
			writeHigh = !writeHigh;

		if (writeHigh == false)
			squareWaveBuffer[c] = 0;
		else
			squareWaveBuffer[c] = 255;
	}

	int success = SDL_QueueAudio(songSettings.audDevice, squareWaveBuffer, squareWaveLength);

	if (success != 0)
		std::cout << "\nError outputting to device!!\n";

	SDL_PauseAudioDevice(songSettings.audDevice, 0);

}


void SineWave(int freq, int length)
{
	// Generate sine wave of A4 with enough data to fill one second of an audio buffer.
	std::cout << "Generating sine\n";

	Uint32 sineWaveLength;
	Uint8 *sineWaveBuffer;



	sineWaveLength = (samplespersec / 1000) * length;
	sineWaveBuffer = new Uint8[sineWaveLength];

	int changeSignEveryXCycles = samplespersec / (freq * 2); // 440 = A4.  Value is doubled as the entire square wave has to be 440 hz.
	int stepVal = 256 / changeSignEveryXCycles;

	bool writeHigh = false;


	for (int c = 0; c < sineWaveLength; c++)
	{
		int currentCyclePos = c % changeSignEveryXCycles;

		if (currentCyclePos == 0)
			writeHigh = !writeHigh;

		if (writeHigh == false)
		{
			sineWaveBuffer[c] = stepVal * currentCyclePos;
		}
		else
		{
			sineWaveBuffer[c] = 255 - (stepVal * currentCyclePos);
		}

		// test distortion:
		// sineWaveBuffer[c] = Clamp(sineWaveBuffer[c], 0, 255);
			
	}

	int success = SDL_QueueAudio(songSettings.audDevice, sineWaveBuffer, sineWaveLength);

	if (success != 0)
		std::cout << "\nError outputting to device!!\n";

	SDL_PauseAudioDevice(songSettings.audDevice, 0);

}





void SawtoothWave(int freq, int length)
{
	// Generate sine wave of A4 with enough data to fill one second of an audio buffer.
	std::cout << "Generating sawtooth\n";

	Uint32 sawtoothWaveLength;
	Uint8 *sawtoothWaveBuffer;

	sawtoothWaveLength = (samplespersec / 1000) * length;
	sawtoothWaveBuffer = new Uint8[sawtoothWaveLength];

	int changeSignEveryXCycles = samplespersec / freq; // 440 = A4.  Value is doubled as the entire square wave has to be 440 hz.
	int stepVal = 256 / changeSignEveryXCycles;

	bool writeHigh = false;


	for (int c = 0; c < sawtoothWaveLength; c++)
	{
		int currentCyclePos = c % changeSignEveryXCycles;

			sawtoothWaveBuffer[c] = stepVal * currentCyclePos;

	}


	int success = SDL_QueueAudio(songSettings.audDevice, sawtoothWaveBuffer, sawtoothWaveLength);

	if (success != 0)
		std::cout << "\nError outputting to device!!\n";

	SDL_PauseAudioDevice(songSettings.audDevice, 0);

}



void PlayNoise(int length, bool lowPitch)
{
	// Generate square wave of A4 with enough data to fill one second of an audio buffer.
	std::cout << "Generating noise\n";

	Uint32 waveLength;
	Uint8 *waveBuffer;


	waveLength = (samplespersec / 1000) * length;;
	waveBuffer = new Uint8[waveLength];

	if (lowPitch != true)
	{
		for (int c = 0; c < waveLength; c++)
		{
			waveBuffer[c] = rand() % 255;
		}
	}
	else
	{
		for (int c = 0; c < waveLength; c += 4)
		{
			int val = rand() % 255;

			waveBuffer[c] = val;
			waveBuffer[c + 1] = val;
			waveBuffer[c + 2] = val;
			waveBuffer[c + 3] = val;

		}
	}


	int success = SDL_QueueAudio(songSettings.audDevice, waveBuffer, waveLength);

	/*
	for (int c = 0; c < waveLength; c += 2)
	{
		int val = rand() % 255;

		waveBuffer[c] = val;
		waveBuffer[c + 1] = val;
	}

	success = SDL_QueueAudio(songSettings.audDevice, waveBuffer, waveLength);

	for (int c = 0; c < waveLength; c += 3)
	{
		int val = rand() % 255;

		waveBuffer[c] = val;
		waveBuffer[c + 1] = val;
		waveBuffer[c + 2] = val;
	}

	success = SDL_QueueAudio(songSettings.audDevice, waveBuffer, waveLength);

	for (int c = 0; c < waveLength; c += 4)
	{
		int val = rand() % 255;

		waveBuffer[c] = val;
		waveBuffer[c + 1] = val;
		waveBuffer[c + 2] = val;
		waveBuffer[c + 3] = val;

	}

	success = SDL_QueueAudio(songSettings.audDevice, waveBuffer, waveLength);


	for (int c = 0; c < waveLength; c++)
	{
		waveBuffer[c] = rand() % 255;
	}*/

	if (success != 0)
		std::cout << "\nError outputting to device!!\n";

	SDL_PauseAudioDevice(songSettings.audDevice, 0);

}
