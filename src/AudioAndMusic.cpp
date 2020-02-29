//
//  AudioAndMusic.cpp
//  test
//
//  Created by Morgan on 28/2/20.
//  Copyright © 2020 Morgan. All rights reserved.
//

#include "MusicGen.h"

struct audioSettings audioSettings;
struct songSettings songSettings;
struct internalAudioBuffer internalAudioBuffer;

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
    // DumpBuffer(stream, len, "ReqBuf8.txt");
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
    delete[] drumBuf;
}


void PlayScale()
{
    // flush input buf
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}

    std::string strInputNoteName;
    float fInputFreq;
    AudioData scale;

    // Buffer for total scale (8 notes) in 16-bit format
    int bufLen16 = songSettings.noteLenMS * samplesPerMS * 8;
    // Buffer for total scale (8 notes) in 8-bit format
    int bufLen = bufLen16 * 2;
    Uint8* scaleBuf = new Uint8[bufLen];
  
    for (auto note = Notes.KV.begin(); note != Notes.KV.end(); note++)
    {
        std::cout << note->first << "\n";
    }
    
    std::cout << "\n\nType in the base note name & press enter: ";
    std::getline(std::cin, strInputNoteName);
    
    fInputFreq = Notes.getNoteFreq(strInputNoteName);
    
    // TODO: Fix this
    /*if (fInputFreq == Notes.KV.end()->second)
        std::cout << "\nReceived default note.  Was your input correct?\n";
*/
    
    auto key = majorKey(fInputFreq);
   
    int noteLen = songSettings.noteLenMS * samplesPerMS * 2;

    Sine(key.notes["1st"], songSettings.noteLenMS, halfMag, &scaleBuf[0]);
    Sine(key.notes["2nd"], songSettings.noteLenMS, halfMag, &scaleBuf[noteLen]);
    Sine(key.notes["3rd"], songSettings.noteLenMS, halfMag, &scaleBuf[noteLen * 2]);
    Sine(key.notes["4th"], songSettings.noteLenMS, halfMag, &scaleBuf[noteLen * 3]);
    Sine(key.notes["5th"], songSettings.noteLenMS, halfMag, &scaleBuf[noteLen * 4]);
    Sine(key.notes["6th"], songSettings.noteLenMS, halfMag, &scaleBuf[noteLen * 5]);
    Sine(key.notes["7th"], songSettings.noteLenMS, halfMag, &scaleBuf[noteLen * 6]);
    Sine(key.notes["8th"], songSettings.noteLenMS, halfMag, &scaleBuf[noteLen * 7]);
     
    DumpBuffer(scaleBuf, bufLen, "ScaleSine.txt");
    
    scale.buf = scaleBuf;
    scale.length = bufLen;

    AudioPlayer(scale);
    
    std::map<std::string, float>::iterator note = key.notes.begin();
    while (note != key.notes.end())
    {
        std::cout << "\nFreq of note " << note->first << " is " << note->second;
        note++;
    }

    delete[] scaleBuf;
}


// Playas

void AudioPlayer(AudioData audioData)
{
    //std::cout << "Playing...\n";

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

