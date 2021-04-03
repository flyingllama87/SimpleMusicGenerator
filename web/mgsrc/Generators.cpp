//
//  Generators.cpp
//
//  Created by Morgan on 28/2/20.
//  Copyright © 2020 Morgan. All rights reserved.
//

#include "MusicGen.h"
#include "SDL_assert.h"

// Wave & Noise generators


void Noise(float length, bool lowPitch, Uint8 *inBuf, int magnitude) // expects length in MS
{
    Uint32 waveLength = audioSettings.samplesPerMS * length * 2;
    Uint16 halfMagnitude = magnitude / 2;

    if (lowPitch != true)
    {
        for (int c = 0; c < waveLength; c+=2)
        {
            int16_t noiseVal = (mtRNG() % magnitude) - halfMagnitude;
            inBuf[c] = noiseVal & 0xFF;
            inBuf[c + 1] = noiseVal >> 8;
        }
    }
    else
    {
        for (int c = 0; c < waveLength; c += 8)
        {
            // Should find out how to do proper brownian or pink noise
            int16_t noiseVal = (mtRNG() % magnitude) - halfMagnitude;

            inBuf[c] = noiseVal & 0xFF;
            inBuf[c + 1] = noiseVal >> 8;
            inBuf[c + 2] = noiseVal & 0xFF;
            inBuf[c + 3] = noiseVal >> 8;
            inBuf[c + 4] = noiseVal & 0xFF;
            inBuf[c + 5] = noiseVal >> 8;
            inBuf[c + 6] = noiseVal & 0xFF;
            inBuf[c + 7] = noiseVal >> 8;
        }
    }
}

void SafeSawtooth(float freq, int length, Uint16 magnitude, Uint8 *inBuf, int currPos)
{
    Uint32 sawtoothWaveLength = audioSettings.samplesPerMS * length * 2;
    Uint32 destBufferWaveEnd = currPos + sawtoothWaveLength;

    if (currPos + sawtoothWaveLength > internalAudioBuffer.backBufferLength && internalAudioBuffer.inited)
        destBufferWaveEnd = internalAudioBuffer.backBufferLength;

    int cycleLength = (audioSettings.audSpecHave.freq * 2) / freq;
    #ifdef DEBUG_AUDIO
        std::cout << "\n\n** GENERATOR DIVIDE BY ZERO CHECK ** \n";
        if (cycleLength == 0)
        {
            std::cout << "(audioSettings.audSpecHave.freq * 2) / freq: " << audioSettings.audSpecHave.freq << " " << freq << "\n";
            SDL_assert(cycleLength == 0);
        }
    #endif
    float stepVal = (float)magnitude / (float)cycleLength;
    Uint16 halfMagnitude = magnitude / 2;

    for (int c = currPos; c < (destBufferWaveEnd - 2); c+=2)
    {
        int currentCyclePos = c % cycleLength;
        
        int16_t sawVal = ((float)stepVal * (float)currentCyclePos) - halfMagnitude;
        inBuf[c] = sawVal & 0xFF;
        inBuf[c + 1] = sawVal >> 8;
    }
}

void SafeSquare(float freq, int length, int magnitude, Uint8 *inBuf, int currPos)
{

    Uint32 squareWaveLength = audioSettings.samplesPerMS * length * 2;
    Uint32 destBufferWaveEnd = currPos + squareWaveLength;

    if (currPos + squareWaveLength >= internalAudioBuffer.backBufferLength && internalAudioBuffer.inited)
        destBufferWaveEnd = internalAudioBuffer.backBufferLength;

    // Change X twice as much as the frequency for complete square cycle
    int changeSignEveryXCycles = audioSettings.audSpecHave.freq / (freq * 2);
    bool writeHigh = false;
    Uint16 halfMagnitude = magnitude / 2;

    for (int c = currPos; c < (destBufferWaveEnd - 1); c+=2)
    {
        if ((c/2) % changeSignEveryXCycles == 0)
            writeHigh = !writeHigh;

        if (writeHigh)
        {
            int16_t squareVal = halfMagnitude;
            inBuf[c] = squareVal & 0xFF;
            inBuf[c + 1] = squareVal >> 8;
        }
        else
        {
            int16_t squareVal = halfMagnitude * -1;
            inBuf[c] = squareVal & 0xFF;
            inBuf[c + 1] = squareVal >> 8;
        }
    }
}

void SlideSquare(float startFreq, float endFreq, int length, int magnitude, Uint8* inBuf, int currPos)
{
    float freqDiff = startFreq - endFreq;

    Uint32 squareWaveLength = audioSettings.samplesPerMS * length * 2;
    Uint32 destBufferWaveEnd = currPos + squareWaveLength;

    if (currPos + squareWaveLength >= internalAudioBuffer.backBufferLength && internalAudioBuffer.inited)
        destBufferWaveEnd = internalAudioBuffer.backBufferLength;

    bool writeHigh = false;
    Uint16 halfMagnitude = magnitude / 2;

    for (int c = currPos; c < (destBufferWaveEnd - 1); c += 2)
    {
        float playedRatio = (float)(squareWaveLength - (c - currPos)) / (float)squareWaveLength;

        float currFreq = endFreq + (freqDiff * playedRatio);

        // Change X twice as much as the frequency for complete square cycle
        int changeSignEveryXCycles = audioSettings.audSpecHave.freq / (currFreq * 2);

        if ((c / 2) % changeSignEveryXCycles == 0)
            writeHigh = !writeHigh;

        if (writeHigh)
        {
            int16_t squareVal = halfMagnitude;
            inBuf[c] = squareVal & 0xFF;
            inBuf[c + 1] = squareVal >> 8;
        }
        else
        {
            int16_t squareVal = halfMagnitude * -1;
            inBuf[c] = squareVal & 0xFF;
            inBuf[c + 1] = squareVal >> 8;
        }        
    }
}

// Expects length in ms
void SafeSine(float freq, int length, Uint16 magnitude, Uint8 *inBuf, int currPos)
{

    Uint32 sineWaveLength = audioSettings.samplesPerMS * length * 2;
    Uint32 destBufferWaveEnd = currPos + sineWaveLength;

    if (currPos + sineWaveLength > internalAudioBuffer.backBufferLength && internalAudioBuffer.inited)
        destBufferWaveEnd = internalAudioBuffer.backBufferLength;

    Uint16 halfMagnitude = magnitude / 2;

    // Samples per sine cycle:
    float samplesPerCycle = (float)(audioSettings.audSpecHave.freq * 2) / freq;
    float radsPerStep = (2.0 * M_PI) / samplesPerCycle;

    // std::cout << "\nsamplesPerCycle: " << samplesPerCycle << "\n";

    for (int c = currPos; c < (destBufferWaveEnd - 1); c+=2)
    {
        int16_t sineVal = (sin((float)c * radsPerStep)) * halfMagnitude;
        inBuf[c] = sineVal & 0xFF;
        inBuf[c + 1] = sineVal >> 8;
    }
}

// Expects length in ms
AudioData Silence(float length)
{
    AudioData returnAD;
    int bufLength = length * audioSettings.samplesPerMS;

    returnAD.length = bufLength;
    returnAD.buf = new Uint8[bufLength]();

    return returnAD;
}


// *** EFFECTS ***

// Expects length in total number of bytes.
void SafeFadeOut(Uint8* buffer, int numOfBytes, int currPos)
{
    int destBufferFadeOutEndPos = numOfBytes + currPos;
    
    if (destBufferFadeOutEndPos > internalAudioBuffer.backBufferLength && internalAudioBuffer.inited)
        destBufferFadeOutEndPos = internalAudioBuffer.backBufferLength;
    
    for (int c = currPos; c < destBufferFadeOutEndPos; c+=2)
    {
        // Use the next line to calculate how much we need to reduce the signal by
        float cycleLengthRatio = (float)(numOfBytes - (c - currPos)) / (float)numOfBytes;
        // Convert the input buffer to 16 bit, apply the fade to the current value and convert back to 8 bit buffer
        int16_t bufCurVal16 = (int16_t)(((buffer[c+1] & 0xFF) << 8) | (buffer[c] & 0xFF));
        bufCurVal16 = (int16_t)((float)bufCurVal16 * cycleLengthRatio);
        buffer[c] = bufCurVal16 & 0xFF;
        buffer[c + 1] = bufCurVal16 >> 8;
    }
}


// Expects length in total number of bytes.
void SafeFadeIn(Uint8* buffer, int numOfBytes, int currPos)
{
    int destBufferFadeOutEndPos = numOfBytes + currPos;

    if (destBufferFadeOutEndPos > internalAudioBuffer.backBufferLength && internalAudioBuffer.inited)
        destBufferFadeOutEndPos = internalAudioBuffer.backBufferLength;

    for (int c = currPos; c < destBufferFadeOutEndPos; c += 2)
    {
        float cycleLengthRatio = (float)(c - currPos) / (float)numOfBytes;
        // Convert the input buffer to 16 bit, apply the fade to the current value and convert back to 8 bit buffer
        int16_t bufCurVal16 = (int16_t)(((buffer[c + 1] & 0xFF) << 8) | (buffer[c] & 0xFF));
        bufCurVal16 = (int16_t)((float)bufCurVal16 * cycleLengthRatio);
        buffer[c] = bufCurVal16 & 0xFF;
        buffer[c + 1] = bufCurVal16 >> 8;
    }
}

void SafeLead(float freq, int length, Uint16 magnitude, Uint8* inBuf, int currPos)
{
    if (songSettings.leadSawtooth)
        SafeSawtooth(freq, length, magnitude, inBuf, currPos);
    else if (songSettings.leadSine)
        SafeSine(freq, length, magnitude * 2, inBuf, currPos);
    else
        SafeSquare(freq, length, magnitude, inBuf, currPos);

}

#ifdef ENABLE_DEBUG_FUNCTIONS


int16_t* Noise(float length, bool lowPitch, int magnitude) // expects length in MS
{
    Uint32 waveLength = audioSettings.samplesPerMS * length;;
    int16_t* waveBuffer = new int16_t[waveLength]();

    Uint16 halfMagnitude = magnitude / 2;

    if (lowPitch != true)
    {
        for (int c = 0; c < waveLength; c++)
        {
            waveBuffer[c] = (mtRNG() % magnitude) - halfMagnitude;
        }
    }
    else
    {
        for (int c = 0; c < waveLength; c += 4)
        {
            int val = (mtRNG() % magnitude) - halfMagnitude;

            waveBuffer[c] = val;
            waveBuffer[c + 1] = val;
            waveBuffer[c + 2] = val;
            waveBuffer[c + 3] = val;
        }
    }
    return waveBuffer;
}

void Sawtooth(float freq, int length, Uint16 magnitude, Uint8* inBuf)
{
    Uint32 sawtoothWaveLength = audioSettings.samplesPerMS * length * 2;

    int cycleLength = (audioSettings.audSpecHave.freq * 2) / freq;
    float stepVal = (float)magnitude / (float)cycleLength;
    Uint16 halfMagnitude = magnitude / 2;

    for (int c = 0; c < sawtoothWaveLength; c += 2)
    {
        int currentCyclePos = c % cycleLength;
        int16_t sawVal = ((float)stepVal * (float)currentCyclePos) - halfMagnitude;
        inBuf[c] = sawVal & 0xFF;
        inBuf[c + 1] = sawVal >> 8;
    }
}

int16_t* Sawtooth(float freq, float length, Uint16 magnitude)
{
    Uint32 sawtoothWaveLength = audioSettings.samplesPerMS * length;
    int16_t* sawtoothWaveBuffer = new int16_t[sawtoothWaveLength];

    int cycleLength = audioSettings.audSpecHave.freq / freq;
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

    Uint32 squareWaveLength = audioSettings.samplesPerMS * length;
    int16_t* squareWaveBuffer = new int16_t[squareWaveLength];

    int changeSignEveryXCycles = audioSettings.audSpecHave.freq / (freq * 2);
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
void Square(float freq, int length, int magnitude, Uint8* inBuf)
{
    // Length in bytes
    Uint32 squareWaveLength = audioSettings.samplesPerMS * length * 2;

    // Change X twice as much as the frequency for complete square cycle
    int changeSignEveryXCycles = audioSettings.audSpecHave.freq / (freq * 2);
    bool writeHigh = false;
    Uint16 halfMagnitude = magnitude / 2;

    for (int c = 0; c < squareWaveLength; c += 2)
    {
        if ((c / 2) % changeSignEveryXCycles == 0)
            writeHigh = !writeHigh;

        if (writeHigh)
        {
            int16_t squareVal = halfMagnitude;
            inBuf[c] = squareVal & 0xFF;
            inBuf[c + 1] = squareVal >> 8;
        }
        else
        {
            int16_t squareVal = halfMagnitude * -1;
            inBuf[c] = squareVal & 0xFF;
            inBuf[c + 1] = squareVal >> 8;
        }
    }
}

// Expects length in ms
int16_t* SineWave(float freq, float length, Uint16 magnitude)
{
    /*std::cout << "In sine func.\n"
        "Requested " << length << " ms of audio.\n"
        "sWaveLength is " << audioSettings.samplesPerMS * length << "\n";*/

    Uint16 halfMagnitude = magnitude / 2;
    Uint32 sineWaveLength = audioSettings.samplesPerMS * length;
    int16_t* sineWaveBuffer = new int16_t[sineWaveLength];

    float samplesPerCycle = (float)audioSettings.audSpecHave.freq / freq;
    float radsPerStep = (2.0 * M_PI) / samplesPerCycle;

    // std::cout << "\nsamplesPerCycle: " << samplesPerCycle << "\n";

    for (int c = 0; c < sineWaveLength; c++)
    {
        sineWaveBuffer[c] = (sin((float)c * radsPerStep)) * halfMagnitude;
    }

    return sineWaveBuffer;
}


// Expects length in ms
void Sine(float freq, int length, Uint16 magnitude, Uint8* inBuf)
{
    /*std::cout << "In sine func.\n"
        "Requested " << length << " ms of audio.\n"
        "sWaveLength is " << audioSettings.samplesPerMS * length << "\n";*/

    Uint16 halfMagnitude = magnitude / 2;
    // How many total bytes we need to generate values for:
    Uint32 sineWaveLength = audioSettings.samplesPerMS * length * 2;

    // Samples per sine cycle:
    float samplesPerCycle = (float)(audioSettings.audSpecHave.freq * 2) / freq;
    float radsPerStep = (2.0 * M_PI) / samplesPerCycle;

    // std::cout << "\nsamplesPerCycle: " << samplesPerCycle << "\n";

    for (int c = 0; c < sineWaveLength; c += 2)
    {
        int16_t sineVal = (sin((float)c * radsPerStep)) * halfMagnitude;
        inBuf[c] = sineVal & 0xFF;
        inBuf[c + 1] = sineVal >> 8;
    }
}

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



// Expects length in total number of bytes.
void FadeOut(Uint8* buffer, int numOfBytes)
{
    for (int c = 0; c < numOfBytes; c += 2)
    {
        // Use the next line to calculate how much we need to reduce the signal by
        float cycleLengthRatio = (float)(numOfBytes - c) / (float)numOfBytes;
        // Convert the input buffer to 16 bit, apply the fade to the current value and convert back to 8 bit buffer
        int16_t bufCurVal16 = (int16_t)(((buffer[c + 1] & 0xFF) << 8) | (buffer[c] & 0xFF));
        bufCurVal16 = (int16_t)((float)bufCurVal16 * cycleLengthRatio);
        buffer[c] = bufCurVal16 & 0xFF;
        buffer[c + 1] = bufCurVal16 >> 8;
    }
}


// Expects length in total number of bytes.
void FadeIn(Uint8* buffer, int numOfBytes)
{

    for (int c = 0; c < numOfBytes; c += 2)
    {
        float cycleLengthRatio = (float)c / (float)numOfBytes;
        // Convert the input buffer to 16 bit, apply the fade to the current value and convert back to 8 bit buffer
        int16_t bufCurVal16 = (int16_t)(((buffer[c + 1] & 0xFF) << 8) | (buffer[c] & 0xFF));
        bufCurVal16 = (int16_t)((float)bufCurVal16 * cycleLengthRatio);
        buffer[c] = bufCurVal16 & 0xFF;
        buffer[c + 1] = bufCurVal16 >> 8;
    }
}



void DebugGenerators()
{
    SetupAudio();

    Uint32 waveLength = 1000 * audioSettings.samplesPerMS * 2;
    Uint8* waveBuffer = new Uint8[waveLength]();

    AudioData tempAD;

    tempAD.length = waveLength;
    //tempAD.buf = new Uint8[tempAD.length];
    tempAD.buf = waveBuffer;

    // Noise
    Noise(1000, false, &waveBuffer[0]);
    DumpBuffer(waveBuffer, waveLength, "Noise.txt");
    AudioPlayer(tempAD);

    // Sawtooth
    Sawtooth(440, 1000, halfMag, &waveBuffer[0]);
    DumpBuffer(waveBuffer, waveLength, "Sawtooth.txt");
    AudioPlayer(tempAD);

    // Square
    Square(440, 1000, halfMag, &waveBuffer[0]);
    DumpBuffer(waveBuffer, waveLength, "Square.txt");
    AudioPlayer(tempAD);

    // Sine fade in
    Sine(440, 1000, halfMag, &waveBuffer[0]);
    FadeIn(&waveBuffer[0], waveLength);
    DumpBuffer(waveBuffer, waveLength, "SineWaveFadeIn.txt");
    AudioPlayer(tempAD);

    // Sine
    Sine(440.0F, 1000, halfMag, &waveBuffer[0]);
    DumpBuffer(waveBuffer, waveLength, "SineWave.txt");
    AudioPlayer(tempAD);

    // Sine + FadeOut
    Sine(440, 1000, halfMag, &waveBuffer[0]);
    FadeOut(&waveBuffer[0], waveLength);
    DumpBuffer(waveBuffer, waveLength, "SineWaveFadeOut.txt");
    AudioPlayer(tempAD);

    std::cout << "Testing 'safe' generators\n";

    // Sawtooth
    SafeSawtooth(440, 1000, halfMag, waveBuffer, 0);
    DumpBuffer(waveBuffer, waveLength, "SafeSawtooth.txt");
    AudioPlayer(tempAD);

    // Sawtooth + FadeOut
    SafeSawtooth(440, 1000, halfMag, waveBuffer, 0);
    SafeFadeOut(waveBuffer, waveLength, 0);
    DumpBuffer(waveBuffer, waveLength, "SafeSawtoothFadeOut.txt");
    AudioPlayer(tempAD);

    std::cout << "Testing 'slide' generators\n";

    SlideSquare(440.0F, 220.0F, 1000, halfMag, waveBuffer, 0);
    DumpBuffer(waveBuffer, waveLength, "SlideSquare.txt");
    AudioPlayer(tempAD);

    SlideSquare(440.0F, 400.0F, 1000, halfMag, waveBuffer, 0);
    AudioPlayer(tempAD);

    SlideSquare(440.0F, 440.0F, 1000, halfMag, waveBuffer, 0);
    DumpBuffer(waveBuffer, waveLength, "SlideSquareSameFreq.txt");

    AudioPlayer(tempAD);

    delete[] waveBuffer;
}

void DebugReverb()
{
    SetupAudio();

    Uint32 waveLength = 2000 * audioSettings.samplesPerMS * 2;
    Uint8* waveBuffer = new Uint8[waveLength]();
    short* waveBuffer16 = new short[waveLength / 2]();
    short* waveBuffer16ReverbOut = new short[waveLength / 2]();


    AudioData tempAD;

    tempAD.length = waveLength;
    //tempAD.buf = new Uint8[tempAD.length];
    tempAD.buf = waveBuffer;

    // Sawtooth
    waveBuffer16 = Sawtooth(440, 2000, halfMag);
    c16to8(waveBuffer16, waveLength / 4, waveBuffer);
    DumpBuffer(waveBuffer, waveLength, "Sawtooth16.txt");
    AudioPlayer(tempAD);

    Reverb(waveBuffer16, waveBuffer16ReverbOut, waveLength);
    c16to8(waveBuffer16ReverbOut, waveLength / 4, waveBuffer);
    DumpBuffer(waveBuffer, waveLength, "SawtoothR16.txt");
    AudioPlayer(tempAD);


}
#endif