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

#ifndef _WIN32 || _WIN64
#include <filesystem>
#endif

// Enable this to get a message everytime a destructor is called
#define TRACK_DEALLOC 1

#define samplesPerSec 48000
#define numChannels 1
#define sampleFmt AUDIO_S16LSB
#define samplesBufNum 32768 // Must be power of two.  Must be 4096 for LOADWAV. if samplesPerSec is 8000, use 4096.  if samplesPerSec is 48000 use 32768

const float samplesPerMS = (float)samplesPerSec / 1000.0F;
const float eightBitSamplesPerMS = samplesPerMS * 2.0;

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

// Functions

// Func prototypes

void Menu();
void SetupAudio(bool callback = false);
void DumpBuffer(int16_t* wavBuffer, int length, std::string fileName);
void DumpBuffer(Uint8* wavBuffer, int length, std::string fileName);
void c16to8(int16_t* inBuf, int len, Uint8* outBuf);
void GenAudioStream(void* userdata, Uint8* stream, int len);
void GenMusicStream();

void AudioPlayer(AudioData audioData);

void DebugGenerators();
void DebugGeneratorsNew();
int16_t* Square(float freq, float length, int magnitude);
int16_t* Sawtooth(float freq, float length, Uint16 magnitude);
int16_t* Noise(float length, bool lowPitch, int magnitude = halfMag);
int16_t* SineWave(float freq, float length, Uint16 magnitude);
void Square(float freq, int length, int magnitude, Uint8* inBuf);
void Sawtooth(float freq, int length, Uint16 magnitude, Uint8* inBuf);
void Noise(float length, bool lowPitch, Uint8* inBuf, int magnitude = halfMag);
void Sine(float freq, int length, Uint16 magnitude, Uint8* inBuf);
AudioData Silence(float length);

void FadeIn(int16_t* buffer, int numOfSamples);
void FadeOut(int16_t* buffer, int numOfSamples);
void FadeIn(Uint8* buffer, int numOfSamples);
void FadeOut(Uint8* buffer, int numOfSamples);

void GenDrumBeat(Uint8* drumBuf);
void TestDrums();
AudioData GiveKick();
AudioData GiveHihat();
AudioData GiveSnare();

void PlayScale();

bool is_float_number(const std::string& s);


// Structs

struct audioSettings
{
    SDL_AudioSpec audSpecWant, audSpecHave;
    SDL_AudioDeviceID device;
};
extern audioSettings audioSettings;

struct songSettings
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

    AudioData kickSound;
    AudioData snareSound;
    AudioData hihatSound;

    songSettings()
    {
        this->BPM = 120;
        this->beatsToBar = 4;
        this->keyFreq = 0.0F;
        init();
    }

    songSettings(Uint8 bpm, Uint8 beatsToBar, float keyFreq)
    {
        this->BPM = bpm;
        this->beatsToBar = beatsToBar;
        this->keyFreq = keyFreq;
        init();
    }

    void init()
    {
        //set lengths
        this->barsPerMin = BPM / beatsToBar;
        this->barLenMS = 60000 / barsPerMin; // bar length in ms.
        this->noteLenMS = 60000 / BPM; // Get noteLength in ms. 60000 = 1 min in milliseconds.
        this->halfNoteLenMS = noteLenMS / 2;
        this->quarterNoteLenMS = noteLenMS / 4;
        this->eighthNoteLenMS = noteLenMS / 8;

        // set drum sounds
        this->kickSound = GiveKick();
        this->snareSound = GiveSnare();
        this->hihatSound = GiveHihat();
    }

};
extern songSettings songSettings;

struct internalAudioBuffer
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
};
extern internalAudioBuffer internalAudioBuffer;

struct AudioData16
{
    Uint32 length = 0;
    int16_t* buf;
};

struct majorKey
{
    const float twelthRootOf2 = powf(2.0f, 1.0f / 12.0f);

    float keyFreq = 0.0f;

    std::map<std::string, float> notes;
    std::vector<float> freqs;

    majorKey(float freq)
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

        for (std::map<std::string,float>::iterator it = notes.begin(); it != notes.end(); it++)
        {
            freqs.push_back(it->second);
        }
    }
};

struct minorKey
{
    const float twelthRootOf2 = powf(2.0f, 1.0f / 12.0f);

    float keyFreq = 0.0f;  // Will be overwritten during construction

    std::map<std::string, float> notes;
    std::vector<float> freqs;

    minorKey(float freq)
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

        for (std::map<std::string,float>::iterator it = notes.begin(); it != notes.end(); it++)
        {
            freqs.push_back(it->second);
        }
    }
};

// enums

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
        std::make_pair("C2", 65.41f)
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

