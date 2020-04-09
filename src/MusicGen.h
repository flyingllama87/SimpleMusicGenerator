//
//  MusicGen.h
//  test
//
//  Created by Morgan on 28/2/20.
//  Copyright © 2020 Morgan. All rights reserved.
//

#include <iostream>
#include <SDL.h>
#include <string>
#include <vector>
#include <stdlib.h>
#include <fstream>
#include <cmath>
#include <map>
#include <iterator>
#include <list>
#include <functional>
#include <algorithm>
#include <random>

#ifndef _WIN32 || _WIN64
#include <filesystem>
#endif

// #define DEBUG_AUDIO 1
// #define PRINT_PATHS 1
// #define DEBUG_BUFFERS 1

#define numChannels 1
#define sampleFmt AUDIO_S16LSB

// Magnitude settings.  Relies on 16 bit ints at the moment.  Should switch to float vals?
#define fullMag 65535
#define halfMag 32767
#define qtrMag 16383

// This struct needs to be at the top for reasons

struct AudioData
{
    Uint32 length = 0;
    Uint8* buf;
};

enum Key { Major, Minor };


// Functions prototypes

// Menu / CLI specific
void Menu();
void ChangeSongSettingsCLI(); // CLI for changing song & audio settings.

// Audio specific
int InitSDL();
void ConfigSong(int bpm, char note, int scale, bool lofi);
void RandomConfig();
void SetupAudio(bool callback = false);
void DumpBuffer(int16_t* wavBuffer, int length, std::string fileName);
void DumpBuffer(Uint8* wavBuffer, int length, std::string fileName);
void c16to8(int16_t* inBuf, int len, Uint8* outBuf);
void GenAudioStream(void* userdata, Uint8* stream, int len);
void GenMusicStream();
void AudioPlayer(AudioData audioData);
void SafeMemCopy(Uint8* destBuf, Uint8* srcBuf, Uint32 srcBufLen, int c, int destBufLen);
int WriteMusicBuffer(void* ptr);


// Wave & Sound Generators
void DebugGenerators();
void DebugGeneratorsNew();
int16_t* Square(float freq, float length, int magnitude);
int16_t* Sawtooth(float freq, float length, Uint16 magnitude);
int16_t* Noise(float length, bool lowPitch, int magnitude = halfMag);
int16_t* SineWave(float freq, float length, Uint16 magnitude);
void Square(float freq, int length, int magnitude, Uint8* inBuf);
void SafeSquare(float freq, int length, int magnitude, Uint8 *inBuf, int currPos);
void Sawtooth(float freq, int length, Uint16 magnitude, Uint8* inBuf);
void SafeSawtooth(float freq, int length, Uint16 magnitude, Uint8 *inBuf, int currPos);
void Noise(float length, bool lowPitch, Uint8* inBuf, int magnitude = halfMag);
void Sine(float freq, int length, Uint16 magnitude, Uint8* inBuf);
void SafeSine(float freq, int length, Uint16 magnitude, Uint8 *inBuf, int currPos);
void SafeLead(float freq, int length, Uint16 magnitude, Uint8* inBuf, int currPos);
AudioData Silence(float length);

// Effects
void FadeIn(int16_t* buffer, int numOfSamples);
void FadeOut(int16_t* buffer, int numOfSamples);
void FadeIn(Uint8* buffer, int numOfSamples);
void FadeOut(Uint8* buffer, int numOfSamples);
void SafeFadeOut(Uint8* buffer, int numOfBytes, int currPos);
void SafeFadeIn(Uint8* buffer, int numOfBytes, int currPos);

// Music
void PlayScale();
Key SwitchKey(Key key);

// Drums
void GenDrumBeat(Uint8* drumBuf, int drumBufLength);
void TestDrums();
AudioData GiveKick();
AudioData GiveHihat();
AudioData GiveSnare();

// Bass
void GenBassTrack(Uint8* bassBuf, int bassBufLength);
void GenLeadTrack(Uint8* leadBuf, int leadBufLength);

// Validation functions.
bool stringIsFloat (const std::string& s);
bool stringIsInt (const std::string& s);
bool IsANote(std::string str);

// Structs

struct audioSettings
{
    SDL_AudioSpec audSpecWant, audSpecHave;
    SDL_AudioDeviceID device;
    float samplesPerMS;
    float bytesPerMS;
    bool inited = false;
    
    audioSettings()
    {
        SDL_memset(&audSpecWant, 0, sizeof(audSpecWant));
        SDL_memset(&audSpecHave, 0, sizeof(audSpecHave));
        audSpecWant.freq = 48000;
        audSpecWant.samples = 32768;
    }
    
    void Init(bool callback)
    {
        //audioSettings.audSpecWant.freq = samplesPerSec;
        audSpecWant.format = sampleFmt;
        audSpecWant.channels = 1;
        //audioSettings.audSpecWant.samples = samplesBufNum;
        if (callback) {
            audSpecWant.callback = GenAudioStream;
        }
        else {
            audSpecWant.callback = NULL;
        }

        device = SDL_OpenAudioDevice(NULL, 0, &audSpecWant, &audSpecHave, 0);
        
#ifdef DEBUG_AUDIO
        std::cout << "\n\nBit Size: " << SDL_AUDIO_BITSIZE(audSpecHave.format) << "\n";
        std::cout << "Bit Rate/Frequency: " << audSpecHave.freq << "\n";

        if (SDL_AUDIO_ISSIGNED(audSpecHave.format))
            printf("Audio format is signed.\n");
        else
            printf("Audio format is unsigned.\n");

        if (SDL_AUDIO_ISBIGENDIAN(audSpecHave.format))
            printf("Audio format is big endian.\n");
        else
            printf("Audio format is little endian.\n\n");
#endif
        
        SDL_PauseAudioDevice(device, 0);
        
        samplesPerMS = (float)this->audSpecHave.freq / 1000.0F;
        bytesPerMS = samplesPerMS * 2.0;
        
        // Set flag to let other structs kknow this one has been initialised.
        inited = true;
    }
    
    void StopAudio()
    {
        SDL_PauseAudioDevice(device, 1);
        SDL_CloseAudioDevice(device);
    }
};
extern audioSettings audioSettings;

struct userSettings {
    int BPM;
    char note;
    int scale;
    bool lofi;
};
userSettings GetSongSettings();

struct songSettings
{
    int BPM;
    int beatsToBar;
    int barsPerMin;
    int barLenMS; // bar length in ms.
    int dblNoteLenMS;
    int noteLenMS; // Get noteLength in ms. 60000 = 1 min in milliseconds.
    int halfNoteLenMS;
    int qtrNoteLenMS;
    int eighthNoteLenMS;
    int barLenBytes;
    int dblNoteLenBytes;
    int noteLenBytes;
    int halfNoteLenBytes;
    int qtrNoteLenBytes;
    int eighthNoteLenBytes;
    int prevPatternDrums = 1;
    int prevPatternBass = 0;
    int prevPatternLead = 0;
    std::string keyNote;
    Key key;
    bool loFi;
    bool inited;
    bool genDrums;
    bool genBass;
    bool genLead;
    bool leadSine = false;
    bool leadSawtooth = false;
    bool leadSquare = true;
    
    AudioData kickSound;
    AudioData snareSound;
    AudioData hihatSound;
    AudioData kickHatSound;
    AudioData snareHatSound;

    songSettings()
    {
        this->BPM = 220;
        this->beatsToBar = 4;
        this->keyNote = "A";
        this->key = Minor;
        this->loFi = false;
        this->inited = false;
        this->genDrums = true;
        this->genBass = true;
        this->genLead = true;
    }

    void Init() // Must be initialised after audioSettings are initialised.
    {
        if (this->inited == true)
        {
            delete[] kickSound.buf;
            delete[] snareSound.buf;
            delete[] hihatSound.buf;
            delete[] snareHatSound.buf;
            delete[] kickHatSound.buf;
        }
        if (!audioSettings.inited)
            std::cout << "\n\nWARNING: audioSettings not initialised.  This will fail.\n\n";
        //set lengths
        this->barsPerMin = BPM / beatsToBar;
        this->barLenMS = 60000 / barsPerMin; // bar length in ms.
        this->noteLenMS = 60000 / BPM; // Get noteLength in ms. 60000 = 1 min in milliseconds.
        this->dblNoteLenMS = noteLenMS * 2;
        this->halfNoteLenMS = noteLenMS / 2;
        this->qtrNoteLenMS = noteLenMS / 4;
        this->eighthNoteLenMS = noteLenMS / 8;
        this->barLenBytes = barLenMS * audioSettings.bytesPerMS;
        this->dblNoteLenBytes = dblNoteLenMS * audioSettings.bytesPerMS;
		this->noteLenBytes = noteLenMS * audioSettings.bytesPerMS;
		this->halfNoteLenBytes = halfNoteLenMS * audioSettings.bytesPerMS;
		this->qtrNoteLenBytes = qtrNoteLenMS * audioSettings.bytesPerMS;
		this->eighthNoteLenBytes = eighthNoteLenMS * audioSettings.bytesPerMS;

        // set drum sounds
        this->kickSound = GiveKick();
        this->snareSound = GiveSnare();
        this->hihatSound = GiveHihat();

        kickHatSound.length = kickSound.length;
        kickHatSound.buf = new Uint8[kickSound.length];
        memcpy(kickHatSound.buf, kickSound.buf, kickSound.length);
        SDL_MixAudioFormat(kickHatSound.buf, hihatSound.buf, sampleFmt, hihatSound.length, SDL_MIX_MAXVOLUME);

        snareHatSound.length = snareSound.length;
        snareHatSound.buf = new Uint8[snareSound.length];
        memcpy(snareHatSound.buf, snareSound.buf, snareSound.length);
        SDL_MixAudioFormat(snareHatSound.buf, hihatSound.buf, sampleFmt, hihatSound.length, SDL_MIX_MAXVOLUME);
        
        this->inited = true;
    }
    
    ~songSettings()
    {
        std::cout << "\nCalling song Settings destructor\n";
        delete[] kickSound.buf;
        delete[] snareSound.buf;
        delete[] hihatSound.buf;
        delete[] snareHatSound.buf;
        delete[] kickHatSound.buf;
    }
};
extern songSettings songSettings;

struct internalAudioBuffer
{
    int pos;
    int length;
    int backBufferLength;
    Uint8* buf;
    Uint8* backBuf;
    bool inited;
    
    internalAudioBuffer()
    {
        inited = false;
    }
    
    void Init()
    {
        pos = -1;
        InitBackBuffer();
        InitBuffer();
    }
    
    void InitBackBuffer()
    {
        if (this->inited == true)
        {
            delete[] this->backBuf;
        }
        if (!songSettings.inited || !audioSettings.inited)
            std::cout << "\n\nWARNING: songSettings or audioSettings not initialised.  This will fail.\n\n";
        backBufferLength = songSettings.barLenMS * 4 * audioSettings.samplesPerMS * 2; // 4 bars, x2 for bytes
        backBufferLength = backBufferLength + (backBufferLength % 2); // Make length  if it wasn't.
        backBuf = new Uint8[backBufferLength]();
    }

        void InitBuffer()
    {
        if (this->inited == true)
        {
            delete[] this->buf;
        }
        if (!songSettings.inited || !audioSettings.inited)
            std::cout << "\n\nWARNING: songSettings or audioSettings not initialised.  This will fail.\n\n";
        length = songSettings.barLenMS * 4 * audioSettings.samplesPerMS * 2; // 4 bars, x2 for bytes
        length = length + (length % 2); // Make length  if it wasn't.

        buf = new Uint8[length]();
        inited = true;
    }

    ~internalAudioBuffer()
    {
        std::cout << "\nCalling internalAudioBuffer destructor\n";
        delete[] buf;
        delete[] backBuf;

    }
    
};
extern internalAudioBuffer internalAudioBuffer;

struct AudioData16
{
    Uint32 length = 0;
    int16_t* buf;
};

static struct Notes
{
    const std::vector<std::pair<std::string, float>> KV =
    {   std::make_pair("B4", 493.90f),
        std::make_pair("A4", 440.00f),
        std::make_pair("G4", 392.00f),
        std::make_pair("F4", 349.23f),
        std::make_pair("E4", 329.60f),
        std::make_pair("D4", 293.67f),
        std::make_pair("C4", 261.62f),
        std::make_pair("B2", 123.47f),
        std::make_pair("A2", 110.00f),
        std::make_pair("G2", 98.00f),
        std::make_pair("F2", 87.31f),
        std::make_pair("E2", 82.41f),
        std::make_pair("D2", 73.42f),
        std::make_pair("C2", 65.41f),
        std::make_pair("B1", 61.74f),
        std::make_pair("A1", 55.00f),
        std::make_pair("G1", 49.00f),
        std::make_pair("F1", 43.65f),
        std::make_pair("E1", 41.20f),
        std::make_pair("D1", 36.71f),
        std::make_pair("C1", 32.70f),
    };
        
    float getNoteFreq(std::string requestedNote)
    {
        //std::find_if (myvector.begin(), myvector.end(), IsOdd);
        auto it = std::find_if(KV.begin(), KV.end(),
                               [&requestedNote](const std::pair<std::string, float>& element){ return element.first == requestedNote;}
        );
        return it->second;
    }
} Notes;


struct Scale
{
    const float twelthRootOf2 = powf(2.0f, 1.0f / 12.0f);

    float keyFreq = 0.0f;

    std::map<std::string, float> notes;
    std::vector<float> freqs;
    
    Scale(Key key, float freq)
    {
        if (key == Key::Major)
            Major(freq);
        else
            Minor(freq);
    }
    
    // Disable default constructor
    Scale() = delete;
    
    void Major(float freq)
    {
        this->keyFreq = freq;

        notes.insert(std::make_pair("1st", this->keyFreq));
        notes.insert(std::make_pair("2nd", this->keyFreq * powf(twelthRootOf2, 2.0f)));
        notes.insert(std::make_pair("3rd", this->keyFreq * powf(twelthRootOf2, 4.0f)));
        notes.insert(std::make_pair("4th", this->keyFreq * powf(twelthRootOf2, 5.0f)));
        notes.insert(std::make_pair("5th", this->keyFreq * powf(twelthRootOf2, 7.0f)));
        notes.insert(std::make_pair("6th", this->keyFreq * powf(twelthRootOf2, 9.0f)));
        notes.insert(std::make_pair("7th", this->keyFreq * powf(twelthRootOf2, 11.0f)));
        notes.insert(std::make_pair("8th", this->keyFreq * powf(twelthRootOf2, 12.0f)));

        for (std::map<std::string, float>::iterator it = notes.begin(); it != notes.end(); it++)
        {
            freqs.push_back(it->second);
        }
    }
    
    void Minor(float freq)
    {
        this->keyFreq = freq;

        notes.insert(std::make_pair("1st", this->keyFreq));
        notes.insert(std::make_pair("2nd", this->keyFreq * powf(twelthRootOf2, 2.0f)));
        notes.insert(std::make_pair("3rd", this->keyFreq * powf(twelthRootOf2, 3.0f)));
        notes.insert(std::make_pair("4th", this->keyFreq * powf(twelthRootOf2, 5.0f)));
        notes.insert(std::make_pair("5th", this->keyFreq * powf(twelthRootOf2, 7.0f)));
        notes.insert(std::make_pair("6th", this->keyFreq * powf(twelthRootOf2, 8.0f)));
        notes.insert(std::make_pair("7th", this->keyFreq * powf(twelthRootOf2, 10.0f)));
        notes.insert(std::make_pair("8th", this->keyFreq * powf(twelthRootOf2, 12.0f)));

        for (std::map<std::string, float>::iterator it = notes.begin(); it != notes.end(); it++)
        {
            freqs.push_back(it->second);
        }
    }
};