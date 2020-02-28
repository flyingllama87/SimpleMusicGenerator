
#include <iostream>
#include <SDL.h>
#include <string>
#include <vector>
#include <stdlib.h>
#include <fstream>
#include <cmath>

#ifndef _WIN32 || _WIN64
#include <filesystem>
#endif

#define samplesPerSec 8000
#define numChannels 1
#define sampleFmt AUDIO_S16LSB
#define samplesBufNum 4096 // Must be power of two.  Must be 4096 for LOADWAV. if samplesPerSec is 8000, use 4096.  if samplesPerSec is 48000 use 32768
const float samplesPerMS = (float)samplesPerSec / 1000.0F;

// Magnitude settings.  Relies on 16 bit ints at the moment.  Should switch to float vals?
#define fullMag 65535
#define halfMag 32767
#define qtrMag 16383

// Structs

static struct audioSettings
{
    SDL_AudioSpec audSpecWant, audSpecHave;
    SDL_AudioDeviceID device;
} audioSettings;

static struct songSettings
{
    int BPM;
    int beatsToBar;
    int barsPerMin;
    int barLenMS; // bar length in ms.
    int noteLenMS; // Get noteLength in ms. 60000 = 1 min in milliseconds.
    int halfNoteLenMS;
    int quarterNoteLenMS;
    int eighthNoteLenMS;
    float keyFreq; // Middle C

    songSettings()
    {
        this->BPM = 120;
        this->beatsToBar = 4;
        this->keyFreq = 261.625F;
        setLengths();
    }

    songSettings(Uint8 bpm, Uint8 beatsToBar, float keyFreq)
    {
        this->BPM = bpm;
        this->beatsToBar = beatsToBar;
        this->keyFreq = keyFreq;
        setLengths();
    }

    void setLengths()
    {
        this->barsPerMin = BPM / beatsToBar;
        this->barLenMS = 60000 / barsPerMin; // bar length in ms.
        this->noteLenMS = 60000 / BPM; // Get noteLength in ms. 60000 = 1 min in milliseconds.
        this->halfNoteLenMS = noteLenMS / 2;
        this->quarterNoteLenMS = noteLenMS / 4;
        this->eighthNoteLenMS = noteLenMS / 8;
    }

} songSettings;

static struct internalAudioBuffer
{
    int pos;
    int length;
    Uint8* buf;

    internalAudioBuffer()
    {
        length = songSettings.barLenMS * 4 * samplesPerMS;
        pos = -1;
        buf = new Uint8[length]();
    }


} internalAudioBuffer;

struct AudioData
{
    Uint32 length = 0;
    Uint8* buf;
};

struct AudioData16
{
    Uint32 length = 0;
    int16_t* buf;
};

struct majorKey
{
    const float twelthRootOf2 = powf(2.0f, 1.0f / 12.0f);

    float keyFreq = 0.0f;

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
        fourthInterval = this->keyFreq * powf(twelthRootOf2, 5.0f);
        fifthInterval = this->keyFreq * powf(twelthRootOf2, 7.0f);
        sixthInterval = this->keyFreq * powf(twelthRootOf2, 9.0f);
        seventhInterval = this->keyFreq * powf(twelthRootOf2, 11.0f);
        octave = this->keyFreq * powf(twelthRootOf2, 12.0f);
    }
};

struct minorKey
{
    const float twelthRootOf2 = powf(2.0f, 1.0f / 12.0f);

    float keyFreq = 0.0f;  // Will be overwritten during construction

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
        fifthInterval = this->keyFreq * powf(twelthRootOf2, 6.0f);
        sixthInterval = this->keyFreq * powf(twelthRootOf2, 8.0f);
        seventhInterval = this->keyFreq * powf(twelthRootOf2, 10.0f);
        octave = this->keyFreq * powf(twelthRootOf2, 12.0f);
    }
};

// enums

static struct Notes
{
    // Lead base freqs
    float const B4Freq = 493.90f;
    float const A4Freq = 440.0f;
    float const G4Freq = 392.0f;
    float const F4Freq = 349.23;
    float const E4Freq = 329.6f;
    float const D4Freq = 293.67f;
    float const C4Freq = 261.623f;

    // Bass base freqs
    float const B2Freq = 123.47f;
    float const A2Freq = 110.00f;
    float const G2Freq = 98.00f;
    float const F2Freq = 87.31f;
    float const E2Freq = 82.41f;
    float const D2Freq = 73.42f;
    float const C2Freq = 65.41f;
} Notes;


// Func prototypes

void Menu();
void SetupAudio(bool callback = false);
void DumpBuffer(int16_t* wavBuffer, float length, std::string fileName);
void DumpBuffer(Uint8* wavBuffer, float length, std::string fileName);
void c16to8(int16_t* inBuf, int len, Uint8* outBuf);
void GenAudioStream(void* userdata, Uint8* stream, int len);
void GenMusicStream();

void AudioPlayer(AudioData audioData);

void DebugGenerators();
int16_t* Square(float freq, float length, int magnitude);
int16_t* Sawtooth(float freq, float length, Uint16 magnitude);
int16_t* Noise(float length, bool lowPitch, int magnitude = halfMag);
int16_t* SineWave(float freq, float length, Uint16 magnitude);
AudioData Silence(float length);

void FadeIn(int16_t* buffer, int numOfSamples);
void FadeOut(int16_t* buffer, int numOfSamples);

void GenDrumBeat(Uint8* drumBuf);
void TestDrums();
AudioData GiveKick();
AudioData GiveHihat();
AudioData GiveSnare();

void PlayScale();

bool is_float_number(const std::string& s);

// main etc.
int main(int argc, char* argv[])
{
    srand(0);
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        std::cout << "Could not initialise SDL";
    }
    else
    {
        std::cout << "SDL initialised!!\n";
    }

    std::cout << "BPM: " << songSettings.BPM << "\n";
    std::cout << "noteLength(ms): " << songSettings.noteLenMS << "\n";

    Menu();

    return 0;
}

void Menu()
{

    while (true) {
        std::cout << "\n\n"
            "Press a to setup audio without callback\n"
            "Press b to setup audio with callback\n"
            "Press c to test wave/noise/effects generators\n"
            "Press d to test drums\n"
            "Press e to play a major scale\n"
            "Press q to quit\n\nAnswer: ";

        char menuInput = std::cin.get();

        if (menuInput == 'a')
            SetupAudio();
        else if (menuInput == 'b')
            SetupAudio(true);
        else if (menuInput == 'c')
            DebugGenerators();
        else if (menuInput == 'd')
            TestDrums();
        else if (menuInput == 'e')
            PlayScale();
        else if (menuInput == 'q')
        {
            SDL_Quit();
            exit(0);
        }
        std::cin.get();
    }

}

void SetupAudio(bool callback)
{
    SDL_memset(&audioSettings.audSpecWant, 0, sizeof(audioSettings.audSpecWant));

    audioSettings.audSpecWant.freq = samplesPerSec;
    audioSettings.audSpecWant.format = sampleFmt;
    audioSettings.audSpecWant.channels = numChannels;
    audioSettings.audSpecWant.samples = samplesBufNum;
    if (callback) {
        audioSettings.audSpecWant.callback = GenAudioStream;
    }
    else {
        audioSettings.audSpecWant.callback = NULL;
    }

    audioSettings.device = SDL_OpenAudioDevice(NULL, 0, &audioSettings.audSpecWant, &audioSettings.audSpecHave, 0);

    std::cout << "\n\nBit Size: " << SDL_AUDIO_BITSIZE(audioSettings.audSpecHave.format) << "\n";
    std::cout << "Bit Rate/Frequency: " << audioSettings.audSpecHave.freq << "\n";

    if (SDL_AUDIO_ISSIGNED(audioSettings.audSpecHave.format))
        printf("Audio format is signed.\n");
    else
        printf("Audio format is unsigned.\n");

    if (SDL_AUDIO_ISBIGENDIAN(audioSettings.audSpecHave.format))
        printf("Audio format is big endian.\n");
    else
        printf("Audio format is little endian.\n");

    SDL_PauseAudioDevice(audioSettings.device, 0);
}


// len is number of bytes not number of samples requested.
void GenAudioStream(void* userdata, Uint8* stream, int len)
{

    /*
    int noOfSamplesRequested = len / 2;

    std::cout << "No. of bytes requested: " << len << "\nLength of audio requested (ms): " << noOfSamplesRequested / samplesPerMS << "\n";
    std::cout << "No. of samples requested: " << len / sizeof(int16_t) << "\n";

    std::cout << "\nIn gen func.\n"
    "No of samples requested: " << noOfSamplesRequested << ".\n"
    "Samples per ms: " << samplesPerMS << ".\n"
    "ms of audio requested: " << (float)noOfSamplesRequested / samplesPerMS << "\n\n";
     */

     //int16_t *sineBuffer = SineWave(440.0F, (float)noOfSamplesRequested / samplesPerMS, halfMag);

    int bytesTillIntBufEnd = internalAudioBuffer.length - internalAudioBuffer.pos;

    if (internalAudioBuffer.pos == -1) // Fill audio buffer on first run.
    {
        GenMusicStream();
        memcpy(stream, &internalAudioBuffer.buf[internalAudioBuffer.pos], len);
        internalAudioBuffer.pos += len;
    }
    else if (bytesTillIntBufEnd < len) // We are near the end of the internal buffer
    {
        // Copy the remaining data from the internal buffer
        memcpy(stream, &internalAudioBuffer.buf[internalAudioBuffer.pos], bytesTillIntBufEnd);
        internalAudioBuffer.pos += bytesTillIntBufEnd;
        // Calculate how much extra data we need
        int extraBytesRequired = len - bytesTillIntBufEnd;
        // Generate a new drum beat / music track
        GenMusicStream();
        // fill the stream buffer with whatever is required from the NEW drum buffer
        memcpy(&stream[bytesTillIntBufEnd], &internalAudioBuffer.buf[internalAudioBuffer.pos], extraBytesRequired);
        internalAudioBuffer.pos += extraBytesRequired;
    }
    else // We still have data in the internal buffer to use
    {
        memcpy(stream, &internalAudioBuffer.buf[internalAudioBuffer.pos], len);
        internalAudioBuffer.pos += len;
    }

    // c16to8(waveBuffer, noOfSamplesRequested, stream);
    //DumpBuffer(stream, len, "ReqBuf8.txt");
    //DumpBuffer(waveBuffer, len, "ReqBuf16.txt");
}

void GenMusicStream()
{
    // Clear internal buffer
    std::fill_n(internalAudioBuffer.buf, internalAudioBuffer.length, 0);

    // Drums
    Uint8* drumBuf = new Uint8[internalAudioBuffer.length] ();
    GenDrumBeat(drumBuf);

    SDL_MixAudioFormat(internalAudioBuffer.buf, drumBuf, sampleFmt, internalAudioBuffer.length, SDL_MIX_MAXVOLUME);
}

void GenDrumBeat(Uint8 *drumBuf)
{
    AudioData kick = GiveKick();
    AudioData hihat = GiveHihat();
    AudioData snare = GiveSnare();

    //std::fill_n(drumBuf, internalAudioBuffer.length, 0);

    int beatCount = 1;
    int barCount = 1;

    int pickRandDrumPattern = rand() % 7;
    std::cout << "\nPlaying drum pattern: " << pickRandDrumPattern << "\n";

    // pickRandDrumPattern = 6;

    switch (pickRandDrumPattern) {
    case 0: // kick x2, hat x2, snare x2, hat x2
        for (int c = 0; c < internalAudioBuffer.length; c += (songSettings.halfNoteLenMS * samplesPerMS))
        {
            if (beatCount == 1 || beatCount == 2)
                memcpy(&drumBuf[c], kick.buf, kick.length);
            else if (beatCount == 3 || beatCount == 4 || beatCount == 7)
                memcpy(&drumBuf[c], hihat.buf, hihat.length);
            else if (beatCount == 5 || beatCount == 6)
                memcpy(&drumBuf[c], snare.buf, snare.length);
            else
            {
                memcpy(&drumBuf[c], hihat.buf, hihat.length);
                beatCount = 0;
                barCount++;
            }
            beatCount++;
        }
        break;
    case 1: // classic kick, hat, snare, hat
        for (int c = 0; c < internalAudioBuffer.length; c += songSettings.noteLenMS * samplesPerMS)
        {
            if (beatCount == 1)
                memcpy(&drumBuf[c], kick.buf, kick.length);
            else if (beatCount == 2)
                memcpy(&drumBuf[c], hihat.buf, hihat.length);
            else if (beatCount == 3)
                memcpy(&drumBuf[c], snare.buf, snare.length);
            else if (beatCount == 4)
            {
                memcpy(&drumBuf[c], hihat.buf, hihat.length);
                beatCount = 0;
                barCount++;
            }
            beatCount++;
        }
        break;
    case 2: //kick, kick, snare!
        for (int c = 0; c < internalAudioBuffer.length; c += songSettings.noteLenMS * samplesPerMS)
        {
            if (beatCount == 1)
                memcpy(&drumBuf[c], kick.buf, kick.length);
            else if (beatCount == 2)
                memcpy(&drumBuf[c], kick.buf, kick.length);
            else if (beatCount == 3)
                memcpy(&drumBuf[c], snare.buf, snare.length);
            else
            {
                beatCount = 0;
                barCount++;
            }
            beatCount++;
        }
        break;
    case 3: // Kick & snare with hi-hat every half beat
    {
        Uint8* kickHatBuf = new Uint8[kick.length];
        memcpy(kickHatBuf, kick.buf, kick.length);
        SDL_MixAudioFormat(kickHatBuf, hihat.buf, sampleFmt, hihat.length, SDL_MIX_MAXVOLUME);

        Uint8* snareHatBuf = new Uint8[snare.length];
        memcpy(snareHatBuf, snare.buf, snare.length);
        SDL_MixAudioFormat(snareHatBuf, hihat.buf, sampleFmt, hihat.length, SDL_MIX_MAXVOLUME);

        for (int c = 0; c < internalAudioBuffer.length; c += (songSettings.halfNoteLenMS * samplesPerMS))
        {
            if (beatCount == 1)
                memcpy(&drumBuf[c], kickHatBuf, kick.length);
            else if (beatCount == 8)
            {
                memcpy(&drumBuf[c], hihat.buf, hihat.length);
                beatCount = 0;
                barCount++;
            }
            else if (beatCount == 5)
                memcpy(&drumBuf[c], snareHatBuf, snare.length);
            else
                memcpy(&drumBuf[c], hihat.buf, hihat.length);
            beatCount++;
        }
        break;
    }
    case 4: // Kick & snare, 2 beats each
        for (int c = 0; c < internalAudioBuffer.length; c += songSettings.noteLenMS * samplesPerMS)
        {
            if (beatCount == 1)
                memcpy(&drumBuf[c], kick.buf, kick.length);
            else if (beatCount == 3)
                memcpy(&drumBuf[c], snare.buf, snare.length);
            else if (beatCount == 4)
                beatCount = 0;
            beatCount++;
        }
        break;
    case 5: // Just kicks!
        for (int c = 0; c < internalAudioBuffer.length; c += songSettings.noteLenMS * samplesPerMS)
        {
            if (beatCount == 4)
                beatCount = 0;
            memcpy(&drumBuf[c], kick.buf, kick.length);
            beatCount++;
        }
        break;
    case 6: // hi hat rolls - WTF is even going on here?
    {
        bool hatRoll = false;

        Uint8* kickHatBuf = new Uint8[kick.length];
        memcpy(kickHatBuf, kick.buf, kick.length);
        SDL_MixAudioFormat(kickHatBuf, hihat.buf, sampleFmt, hihat.length, SDL_MIX_MAXVOLUME);

        Uint8* snareHatBuf = new Uint8[snare.length];
        memcpy(snareHatBuf, snare.buf, snare.length);
        SDL_MixAudioFormat(snareHatBuf, hihat.buf, sampleFmt, hihat.length, SDL_MIX_MAXVOLUME);

        for (int c = 0; c < internalAudioBuffer.length; c += songSettings.quarterNoteLenMS * samplesPerMS)
        {
            if (hatRoll && (beatCount == 5 ||
                beatCount == 6 ||
                beatCount == 7 ||
                beatCount == 8 ||
                beatCount == 13))
                memcpy(&drumBuf[c], hihat.buf, hihat.length);

            if (beatCount == 1)
                memcpy(&drumBuf[c], kickHatBuf, kick.length);

            if (beatCount == 9)
                memcpy(&drumBuf[c], snareHatBuf, snare.length);

            if (!hatRoll && (beatCount == 5 ||
                beatCount == 13
                ))
                memcpy(&drumBuf[c], hihat.buf, hihat.length);

            if (beatCount == 16)
            {
                beatCount = 0;
                barCount++;
                hatRoll = !hatRoll;
            }
            beatCount++;
        }
    }
    break;
    default:
        break;
    }

    internalAudioBuffer.pos = 0;
    //DumpBuffer(drumBuf, internalAudioBuffer.length, "testBuffer,txt");
}

// Playas

void AudioPlayer(AudioData audioData)
{
    std::cout << "Playing...\n";

    int success = SDL_QueueAudio(audioSettings.device, audioData.buf, audioData.length);

    if (success != 0)
        std::cout << "\nError outputting to device!!\n";
}

void PlayWAV(std::string fileName)
{
    Uint32 wavLength;
    Uint8* wavBuffer;

    std::cout << "Playing WAV";

    SDL_LoadWAV(fileName.c_str(), &audioSettings.audSpecWant, &wavBuffer, &wavLength);

    int success = SDL_QueueAudio(audioSettings.device, wavBuffer, wavLength);

    if (success != 0)
        std::cout << "\nError outputting to device!!\n";

    SDL_PauseAudioDevice(audioSettings.device, 0);

}

// Wave & Noise generators

void DebugGenerators()
{
    Uint32 waveLength = 1000 * samplesPerMS;
    int16_t* waveBuffer = new int16_t[waveLength]();

    AudioData tempAD;
    tempAD.length = 1000 * samplesPerMS * 2; // Dbl for 8 bit
    tempAD.buf = new Uint8[tempAD.length];


    // Noise
    waveBuffer = Noise(1000, false);
    DumpBuffer(waveBuffer, waveLength, "Noise.txt");
    c16to8(waveBuffer, waveLength, tempAD.buf);
    AudioPlayer(tempAD);

    // Sawtooth
    waveBuffer = Sawtooth(440, 1000, halfMag);
    DumpBuffer(waveBuffer, waveLength, "Sawtooth.txt");
    c16to8(waveBuffer, waveLength, tempAD.buf);
    AudioPlayer(tempAD);

    // Square
    waveBuffer = Square(440, 1000, halfMag);
    DumpBuffer(waveBuffer, waveLength, "Square.txt");
    c16to8(waveBuffer, waveLength, tempAD.buf);
    AudioPlayer(tempAD);

    // Sine fade in
    waveBuffer = SineWave(440, 1000, halfMag);
    FadeIn(waveBuffer, waveLength);
    DumpBuffer(waveBuffer, waveLength, "SineWaveFadeIn.txt");
    c16to8(waveBuffer, waveLength, tempAD.buf);
    AudioPlayer(tempAD);

    // Sine
    waveBuffer = SineWave(440.0F, 1000, halfMag);
    DumpBuffer(waveBuffer, waveLength, "SineWave.txt");
    c16to8(waveBuffer, waveLength, tempAD.buf);
    AudioPlayer(tempAD);

    // Sine fadeout
    waveBuffer = SineWave(440, 1000, halfMag);
    FadeOut(waveBuffer, waveLength);
    DumpBuffer(waveBuffer, waveLength, "SineWaveFadeOut.txt");
    c16to8(waveBuffer, waveLength, tempAD.buf);
    AudioPlayer(tempAD);

    waveBuffer = SineWave(440, 1000, halfMag);
    FadeIn(waveBuffer, waveLength);
    DumpBuffer(waveBuffer, waveLength, "SineWaveFadeIn.txt");
    c16to8(waveBuffer, waveLength, tempAD.buf);
    AudioPlayer(tempAD);

    waveBuffer = SineWave(440, 1000, halfMag);
    FadeOut(waveBuffer, waveLength);
    DumpBuffer(waveBuffer, waveLength, "SineWaveFadeOut.txt");
    c16to8(waveBuffer, waveLength, tempAD.buf);
    AudioPlayer(tempAD);
}


int16_t* Noise(float length, bool lowPitch, int magnitude)
{
    Uint32 waveLength = samplesPerMS * length;;
    int16_t* waveBuffer = new int16_t[waveLength]();

    Uint16 halfMagnitude = magnitude / 2;

    if (lowPitch != true)
    {
        for (int c = 0; c < waveLength; c++)
        {
            waveBuffer[c] = (rand() % magnitude) - halfMagnitude;
        }
    }
    else
    {
        for (int c = 0; c < waveLength; c += 4)
        {
            int val = (rand() % magnitude) - halfMagnitude;

            waveBuffer[c] = val;
            waveBuffer[c + 1] = val;
            waveBuffer[c + 2] = val;
            waveBuffer[c + 3] = val;
        }
    }
    return waveBuffer;
}


int16_t* Sawtooth(float freq, float length, Uint16 magnitude)
{
    Uint32 sawtoothWaveLength = samplesPerMS * length;
    int16_t* sawtoothWaveBuffer = new int16_t[sawtoothWaveLength];

    int cycleLength = samplesPerSec / freq;
    float stepVal = (float)magnitude / (float)cycleLength;
    Uint16 halfMagnitude = magnitude / 2;

    for (int c = 0; c < sawtoothWaveLength; c++)
    {
        int currentCyclePos = c % cycleLength;
        sawtoothWaveBuffer[c] = ((float)stepVal * (float)currentCyclePos) - halfMagnitude;
    }

    return sawtoothWaveBuffer;
}

// Expects length in ms
int16_t* Square(float freq, float length, int magnitude)
{
    //std::cout << "Generating square\n";

    Uint32 squareWaveLength = samplesPerMS * length;
    int16_t* squareWaveBuffer = new int16_t[squareWaveLength];

    int changeSignEveryXCycles = samplesPerSec / (freq * 2);
    bool writeHigh = false;
    Uint16 halfMagnitude = magnitude / 2;

    for (int c = 0; c < squareWaveLength; c++)
    {
        if (c % changeSignEveryXCycles == 0)
            writeHigh = !writeHigh;

        if (writeHigh)
            squareWaveBuffer[c] = halfMagnitude;
        else
            squareWaveBuffer[c] = halfMagnitude * -1;
    }
    return squareWaveBuffer;
}

// Expects length in ms
int16_t* SineWave(float freq, float length, Uint16 magnitude)
{
    /*std::cout << "In sine func.\n"
        "Requested " << length << " ms of audio.\n"
        "sWaveLength is " << samplesPerMS * length << "\n";*/

    Uint16 halfMagnitude = magnitude / 2;
    Uint32 sineWaveLength = samplesPerMS * length;
    int16_t* sineWaveBuffer = new int16_t[sineWaveLength];

    float samplesPerCycle = (float)samplesPerSec / freq;
    float radsPerStep = (2.0 * M_PI) / samplesPerCycle;

    // std::cout << "\nsamplesPerCycle: " << samplesPerCycle << "\n";

    for (int c = 0; c < sineWaveLength; c++)
    {
        sineWaveBuffer[c] = (sin((float)c * radsPerStep)) * halfMagnitude;
    }

    return sineWaveBuffer;
}

// Expects length in ms
AudioData Silence(float length)
{
    AudioData returnAD;
    int bufLength = length * samplesPerMS;

    returnAD.length = bufLength;
    returnAD.buf = new Uint8[bufLength]();

    return returnAD;
}

// Helper functions

// Convert int16_t array to Uint8 little endian
void c16to8(int16_t* inBuf, int len, Uint8* outBuf)
{
    for (auto c = 0; c <= len; c++)
    {
        outBuf[c * 2] = inBuf[c] & 0xFF;
        outBuf[(c * 2) + 1] = inBuf[c] >> 8;
    }
}


void DumpBuffer(Uint8* wavBuffer, float length, std::string fileName)
{
    std::ofstream waveBufferFile;
    waveBufferFile.open(fileName);
    for (int c = 0; c <= length; c++)
    {
        waveBufferFile << wavBuffer[c];
    }
    waveBufferFile.close();

#ifndef _WIN32 || _WIN64
    std::cout << "Current path is " << std::__fs::filesystem::current_path() << '\n';
#endif
}

void DumpBuffer(int16_t* wavBuffer, float length, std::string fileName)
{
    std::ofstream waveBufferFile(fileName.c_str(), std::ios::out | std::ios::binary);
    for (int c = 0; c < (length); c++)
    {
        waveBufferFile.write(reinterpret_cast<const char*>(&wavBuffer[c]), sizeof wavBuffer[c]);
    }
    waveBufferFile.close();

#ifndef _WIN32 || _WIN64
    std::cout << "Current path is " << std::__fs::filesystem::current_path() << '\n';
#endif

}

// Effects

// Expects length in total number of samples.
void FadeOut(int16_t* buffer, int numOfSamples)
{
    for (int c = 0; c < numOfSamples; c++)
    {
        float cycleLengthRatio = (float)(numOfSamples - c) / (float)numOfSamples;
        buffer[c] = (int16_t)((float)buffer[c] * cycleLengthRatio);
    }
}

void FadeIn(int16_t* buffer, int numOfSamples)
{
    for (int c = 0; c < numOfSamples; c++)
    {
        float cycleLengthRatio = (float)c / (float)numOfSamples;
        buffer[c] = (int16_t)((float)buffer[c] * cycleLengthRatio);
    }
}



// Drums

void TestDrums()
{
    AudioData silenceSec = Silence(1000);
    AudioData jiffy = Silence(100);

    AudioData kick = GiveKick();
    //DumpBuffer(Kick.buf, Kick.length, "Kick.txt");
    AudioPlayer(kick);
    AudioPlayer(jiffy);
    AudioPlayer(kick);

    AudioPlayer(silenceSec);

    AudioData hihat = GiveHihat();
    AudioPlayer(hihat);
    AudioPlayer(jiffy);
    AudioPlayer(hihat);

    AudioPlayer(silenceSec);

    AudioData snare = GiveSnare();
    AudioPlayer(snare);
    AudioPlayer(jiffy);
    AudioPlayer(snare);

    DumpBuffer(snare.buf, snare.length, "snare.txt");
}

AudioData GiveKick()
{
    AudioData returnAD;

    // 200ms kick
    Uint16 kickLength = 200;

    Uint32 waveLength = samplesPerMS * kickLength;

    // declare buffers
    int16_t* sineBuffer = new int16_t[waveLength];
    int16_t* waveBuffer = new int16_t[waveLength];

    // Noise
    waveBuffer = Noise(kickLength, true, qtrMag);

    // SineWave
#ifdef _WIN32 || _WIN64
    sineBuffer = SineWave(60, kickLength, halfMag);
#else // Crappy speakers on laptops / mobile devices so need a higher freq kick
    sineBuffer = SineWave(105, kickLength, halfMag);
#endif

    FadeOut(waveBuffer, waveLength);
    FadeOut(sineBuffer, waveLength);

    // Convert to 8-bit buffers
    int waveLength8 = waveLength * 2;

    Uint8* sineBuffer8 = new Uint8[waveLength8];
    c16to8(sineBuffer, waveLength, sineBuffer8);

    Uint8* waveBuffer8 = new Uint8[waveLength8];
    c16to8(waveBuffer, waveLength, waveBuffer8);

    // Mix tracks
    SDL_MixAudioFormat(waveBuffer8, sineBuffer8, sampleFmt, waveLength8, SDL_MIX_MAXVOLUME);

    //memcpy(waveBuffer, sineBuffer, waveLength * sizeof( Uint8 ));

    returnAD.buf = waveBuffer8;
    returnAD.length = waveLength8;

    return returnAD;
}


AudioData GiveHihat()
{
    AudioData returnAD;

    // 150ms kick
    Uint32 waveLength = samplesPerMS * 100;

    // Initialise to 0
    int16_t* waveBuffer = new int16_t[waveLength];
    //std::fill_n(waveBuffer, waveLength, halfMag);

    // Noise
    waveBuffer = Noise(waveLength, false, qtrMag);

    //Fade
    FadeOut(waveBuffer, waveLength);

    // Convert to 8-bit buffers
    int waveLength8 = waveLength * 2;
    Uint8* waveBuffer8 = new Uint8[waveLength8];
    c16to8(waveBuffer, waveLength, waveBuffer8);

    returnAD.buf = waveBuffer8;
    returnAD.length = waveLength8;

    return returnAD;
}

AudioData GiveSnare()
{
    AudioData returnAD;

    Uint32 const waveLength = samplesPerMS * 250;

    // Initialise
    int16_t* waveBuffer = new int16_t[waveLength];
    int16_t* sawBuffer = new int16_t[waveLength];

    // Noise
    sawBuffer = Square(120, waveLength / 2, qtrMag / 4);
    waveBuffer = Noise(waveLength, true, qtrMag);

    FadeOut(waveBuffer, waveLength);
    FadeOut(sawBuffer, waveLength);

    // Convert to 8-bit buffers
    int waveLength8 = waveLength * 2;

    Uint8* sawBuffer8 = new Uint8[waveLength8];
    c16to8(sawBuffer, waveLength, sawBuffer8);

    Uint8* waveBuffer8 = new Uint8[waveLength8];
    c16to8(waveBuffer, waveLength, waveBuffer8);

    // Mix tracks
    SDL_MixAudioFormat(waveBuffer8, sawBuffer8, sampleFmt, waveLength, SDL_MIX_MAXVOLUME);

    returnAD.buf = waveBuffer8;
    returnAD.length = waveLength8;

    return returnAD;
}


void PlayScale()
{
    // flush input buf
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}

    std::string strInputFreq;
    float fInputFreq;
    AudioData scale;

    // Buffer for total scale (8 notes) in 16-bit format
    int bufLen16 = songSettings.noteLenMS * samplesPerMS * 8;
    int16_t* scaleBuf16 = new int16_t[bufLen16] ();

    // Buffer for total scale (8 notes) in 8-bit format
    int bufLen = bufLen16 * 2;
    Uint8* scaleBuf = new Uint8[bufLen];

    // Temporary buffer for notes in 16-bit format... Need to find a better way to do this!!
    int tempBufLen16 = songSettings.noteLenMS * samplesPerMS;
    int16_t* tempBuf16 = new int16_t[tempBufLen16];

    while (true)
    {
        std::cout << "\n\nType in the frequency of the base scale note & press enter: ";
        std::getline(std::cin, strInputFreq);
        //std::cin >> strInputFreq >> std::ws;
        if (is_float_number(strInputFreq))
            break;
    }

    fInputFreq = std::stof(strInputFreq);

    auto key = majorKey(fInputFreq);

    tempBuf16 = SineWave(key.firstInterval, songSettings.noteLenMS, halfMag);
    memcpy(&scaleBuf16[0], tempBuf16, tempBufLen16 * sizeof(int16_t));
    DumpBuffer(scaleBuf16, tempBufLen16, "note.txt");

    tempBuf16 = SineWave(key.secondInterval, songSettings.noteLenMS, halfMag);
    memcpy(&scaleBuf16[tempBufLen16], tempBuf16, tempBufLen16 * sizeof(int16_t));

    tempBuf16 = SineWave(key.thirdInterval, songSettings.noteLenMS, halfMag);
    memcpy(&scaleBuf16[tempBufLen16 * 2], tempBuf16, tempBufLen16 * sizeof(int16_t));

    tempBuf16 = SineWave(key.fourthInterval, songSettings.noteLenMS, halfMag);
    memcpy(&scaleBuf16[tempBufLen16 * 3], tempBuf16, tempBufLen16 * sizeof(int16_t));

    tempBuf16 = SineWave(key.fifthInterval, songSettings.noteLenMS, halfMag);
    memcpy(&scaleBuf16[tempBufLen16 * 4], tempBuf16, tempBufLen16 * sizeof(int16_t));

    tempBuf16 = SineWave(key.sixthInterval, songSettings.noteLenMS, halfMag);
    memcpy(&scaleBuf16[tempBufLen16 * 5], tempBuf16, tempBufLen16 * sizeof(int16_t));

    tempBuf16 = SineWave(key.seventhInterval, songSettings.noteLenMS, halfMag);
    memcpy(&scaleBuf16[tempBufLen16 * 6], tempBuf16, tempBufLen16 * sizeof(int16_t));

    tempBuf16 = SineWave(key.octave, songSettings.noteLenMS, halfMag);
    memcpy(&scaleBuf16[tempBufLen16 * 7], tempBuf16, tempBufLen16 * sizeof(int16_t));

    DumpBuffer(scaleBuf16, bufLen16, "scaleWave16.txt");
    c16to8(scaleBuf16, bufLen16, scaleBuf);

    scale.buf = scaleBuf;
    scale.length = bufLen;

    AudioPlayer(scale);

    std::cout << "\nFreq of note 1: " << key.firstInterval;
    std::cout << "\nFreq of note 2: " << key.secondInterval;
    std::cout << "\nFreq of note 3: " << key.thirdInterval;
    std::cout << "\nFreq of note 4: " << key.fourthInterval;
    std::cout << "\nFreq of note 5: " << key.fifthInterval;
    std::cout << "\nFreq of note 6: " << key.sixthInterval;
    std::cout << "\nFreq of note 7: " << key.seventhInterval;
    std::cout << "\nFreq of note 8: " << key.octave;
}

bool is_float_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && (std::isdigit(*it) || *it == '.' )) ++it;
    return !s.empty() && it == s.end();
}