//
//  AudioAndMusic.cpp
//  test
//
//  Created by Morgan on 28/2/20.
//  Copyright © 2020 Morgan. All rights reserved.
//
#include <Windows.h>
#include "MusicGen.h"


struct audioSettings audioSettings;
struct songSettings songSettings;
struct internalAudioBuffer internalAudioBuffer;

// len is number of bytes not number of samples requested.
void GenAudioStream(void* userdata, Uint8* stream, int len)
{
    static SDL_Thread* backBufferThread;

    // Seed random number gen in callback thread
#ifdef _WIN64
    LARGE_INTEGER cicles;
    QueryPerformanceCounter(&cicles);
    std::srand(cicles.QuadPart);
#endif

    /*
    int noOfSamplesRequested = len / 2;

    std::cout << "No. of bytes requested: " << len << "\nLength of audio requested (ms): " << noOfSamplesRequested / audioSettings.samplesPerMS << "\n";
    std::cout << "No. of samples requested: " << len / sizeof(int16_t) << "\n";
    
	
    std::cout << "\nIn gen func.\n"
    "No of samples requested: " << noOfSamplesRequested << ".\n"
    "Samples per ms: " << audioSettings.samplesPerMS << ".\n"
    "ms of audio requested: " << (float)noOfSamplesRequested / audioSettings.samplesPerMS << "\n";
    */

   // std::cout << "internalAudioBuffer.backBufLength; " << internalAudioBuffer.backBufferLength << "\n";
  //  std::cout << "internalAudioBuffer.length; " << internalAudioBuffer.length << "\n";


    int bytesTillIntBufEnd = internalAudioBuffer.length - internalAudioBuffer.pos;
    if (!audioSettings.inited || !songSettings.inited || !internalAudioBuffer.inited)
    {
        std::cout << "Skipping callback as music functionality is not yet ready";
    }
    else if (internalAudioBuffer.pos == -1) // Fill audio buffer on first run.
    {
        // Fill back buffer
        backBufferThread = SDL_CreateThread(WriteMusicBuffer, "backBuffer", internalAudioBuffer.backBuf);
        if (NULL == backBufferThread)
            printf("SDL_CreateThread failed: %s\n", SDL_GetError());
        //Generate the music stream manually.
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

        int randTestChance = rand() % 100;

        // Generate a new drum beat / music track or repeat the old one?
        if (randTestChance < 20)
        {
            internalAudioBuffer.pos = 0;
            std::cout << "\n   > RNJesus wants the previous section to repeat first... \n";
        }
        else
        {
            if (backBufferThread != NULL)
            {
                // Yield thread
                int retVal = NULL;
                SDL_WaitThread(backBufferThread, &retVal);
                backBufferThread = NULL;

                // Refresh primary buffer
                internalAudioBuffer.InitBuffer();
                if (retVal != NULL)
                {
                    memmove(internalAudioBuffer.buf, internalAudioBuffer.backBuf, min(internalAudioBuffer.length, internalAudioBuffer.backBufferLength));
                    internalAudioBuffer.pos = 0;
                }
                else {
                    std::cout << "***ERROR*** Something happened with the BG thread!!! \n";
                    GenMusicStream();
                }

                // Evaluate switching musical keys or BPM between thread generations
                // switch keys?
                if (randTestChance > 90 && internalAudioBuffer.backBufferLength == internalAudioBuffer.length)
                {
                    // TO DO: Use some music theory, pick the IV, V or vi
                    char note = (rand() % 7) + 65;
                    songSettings.keyNote = note;
                    if (rand() % 2)
                        songSettings.key = Key::Major;
                    else
                        songSettings.key = Key::Minor;

                    std::cout << "\n   > RNJesus wants to change the key to: " << songSettings.keyNote << " " << (songSettings.key == Key::Major ? "Major" : "Minor") << "\n";
                    // reinit
                    songSettings.Init();
                }

                // switch bpm?
                if ((randTestChance > 80 || (songSettings.BPM < 110 && randTestChance % 5 == 0)) && internalAudioBuffer.backBufferLength == internalAudioBuffer.length)
                {
                    songSettings.BPM = ((rand() % 31) * 4) + 116;
                    std::cout << "\n   > RNJesus wants to change the BPM to: " << songSettings.BPM << "\n";
                    // reinit
                    songSettings.Init();
                    internalAudioBuffer.InitBackBuffer();
                }

                // Launch new thread to generate music to backBuffer
                backBufferThread = SDL_CreateThread(WriteMusicBuffer, "backBuffer", internalAudioBuffer.backBuf);
                if (NULL == backBufferThread)
                    printf("SDL_CreateThread failed: %s\n", SDL_GetError());
                
            }
            else
            {
                std::cout << "***ERROR*** Something happened with the BG thread!!! \n";
                GenMusicStream();
            }
        }

        // fill the stream buffer with whatever is required from the NEW music buffer
        memcpy(&stream[bytesTillIntBufEnd], &internalAudioBuffer.buf[internalAudioBuffer.pos], extraBytesRequired);
        internalAudioBuffer.pos += extraBytesRequired;
        internalAudioBuffer.pos -= internalAudioBuffer.pos % 2; // Even up the position if it's odd for some reason (should always be even).
    }
    else // We still have data in the internal buffer to use
    {
        memcpy(stream, &internalAudioBuffer.buf[internalAudioBuffer.pos], len);
        internalAudioBuffer.pos += len;
    }

#ifdef DEBUG_BUFFERS
    DumpBuffer(stream, len, "ReqBuf.txt");
#endif
}


void GenMusicStream()
{
    // Clear internal buffer
    std::fill_n(internalAudioBuffer.buf, internalAudioBuffer.length, 0);

    // Drums
    Uint8* drumBuf = new Uint8[internalAudioBuffer.length] ();
    if (songSettings.genDrums)
        GenDrumBeat(drumBuf, internalAudioBuffer.length);
    
    // Bass
    Uint8* bassBuf = new Uint8[internalAudioBuffer.length] ();
    if (songSettings.genBass)
        GenBassTrack(bassBuf, internalAudioBuffer.length);

    // Lead
    Uint8* leadBuf = new Uint8[internalAudioBuffer.length] ();
    if (songSettings.genLead)
        GenLeadTrack(leadBuf, internalAudioBuffer.length);
    
    internalAudioBuffer.pos = 0;

    if (songSettings.genDrums)
        SDL_MixAudioFormat(internalAudioBuffer.buf, drumBuf, sampleFmt, internalAudioBuffer.length, SDL_MIX_MAXVOLUME);
    
    if (songSettings.genBass)
        SDL_MixAudioFormat(internalAudioBuffer.buf, bassBuf, sampleFmt, internalAudioBuffer.length, SDL_MIX_MAXVOLUME);
    
    if (songSettings.genLead)
        SDL_MixAudioFormat(internalAudioBuffer.buf, leadBuf, sampleFmt, internalAudioBuffer.length, SDL_MIX_MAXVOLUME);
    
#ifdef DEBUG_BUFFERS
    DumpBuffer(drumBuf, internalAudioBuffer.length, "DrumBuffer.txt");
    DumpBuffer(bassBuf, internalAudioBuffer.length, "BassBuffer.txt");
    DumpBuffer(leadBuf, internalAudioBuffer.length, "LeadBuffer.txt");
    DumpBuffer(internalAudioBuffer.buf, internalAudioBuffer.length, "FullBuffer.txt");
    std::cout << "Dumping Buffers!\n";
#endif

    delete[] drumBuf;
    delete[] bassBuf;
    delete[] leadBuf;

    std::cout << "\n";
}


int WriteMusicBuffer(void* ptr)
{

    std::cout << "\nComing up section in 4/4 timing:\n";

    // Seed random number gen in callback thread
#ifdef _WIN64
    LARGE_INTEGER cicles;
    QueryPerformanceCounter(&cicles);
    std::srand(cicles.QuadPart);
#endif

    Uint8* inBuf = (Uint8*)ptr;

    // Clear internal buffer
    std::fill_n(inBuf, internalAudioBuffer.backBufferLength, 0);

    // Drums
    Uint8* drumBuf = new Uint8[internalAudioBuffer.backBufferLength]();
    if (songSettings.genDrums)
        GenDrumBeat(drumBuf, internalAudioBuffer.backBufferLength);

    // Bass
    Uint8* bassBuf = new Uint8[internalAudioBuffer.backBufferLength]();
    if (songSettings.genBass)
        GenBassTrack(bassBuf, internalAudioBuffer.backBufferLength);

    // Lead
    Uint8* leadBuf = new Uint8[internalAudioBuffer.backBufferLength]();
    if (songSettings.genLead)
        GenLeadTrack(leadBuf, internalAudioBuffer.backBufferLength);

    if (songSettings.genDrums)
        SDL_MixAudioFormat(inBuf, drumBuf, sampleFmt, internalAudioBuffer.backBufferLength, SDL_MIX_MAXVOLUME);

    if (songSettings.genBass)
        SDL_MixAudioFormat(inBuf, bassBuf, sampleFmt, internalAudioBuffer.backBufferLength, SDL_MIX_MAXVOLUME);

    if (songSettings.genLead)
        SDL_MixAudioFormat(inBuf, leadBuf, sampleFmt, internalAudioBuffer.backBufferLength, SDL_MIX_MAXVOLUME);

#ifdef DEBUG_BUFFERS
    DumpBuffer(drumBuf, internalAudioBuffer.length, "DrumBuffer.txt");
    DumpBuffer(bassBuf, internalAudioBuffer.length, "BassBuffer.txt");
    DumpBuffer(leadBuf, internalAudioBuffer.length, "LeadBuffer.txt");
    DumpBuffer(inBuf, internalAudioBuffer.length, "FullBuffer.txt");
    std::cout << "Dumping Buffers!\n";
#endif

    delete[] drumBuf;
    delete[] bassBuf;
    delete[] leadBuf;

    return 1;
}


void TestArpeggios()
{
    SetupAudio();

    std::string strInputNoteName;
    float fInputFreq;
    AudioData scale;

    // Buffer for total scale (8 notes) in 8-bit format
    int bufLen = 1500 * audioSettings.bytesPerMS;
    Uint8* scaleBuf = new Uint8[bufLen];

    for (auto note = Notes.KV.begin(); note != Notes.KV.end(); note++)
    {
        std::cout << note->first << "\n";
    }

    std::cout << "\n\nType in the base note name & press enter: ";
    std::getline(std::cin, strInputNoteName);

    fInputFreq = Notes.getNoteFreq(strInputNoteName);

    auto key = Scale(Key::Major, fInputFreq);

    int noteLen = songSettings.noteLenBytes;

    scale.buf = scaleBuf;
    scale.length = bufLen;

    std::cout << "arpeggio with a note length of 64/sec, no slide. \n";
    std::fill_n(scaleBuf, bufLen, 0);
    GenArp(key.notes["1st"], 1000, 64, halfMag, scale.buf, 0, false);
    AudioPlayer(scale);
    Sleep(1500);

    std::cout << "arpeggio with a note length of 64/sec, slide. \n";
    std::fill_n(scaleBuf, bufLen, 0);
    GenArp(key.notes["1st"], 1000, 64, halfMag, scale.buf, 0, true);
    AudioPlayer(scale);
    Sleep(1500);

    std::cout << "arpeggio with a note length of 32/sec, no slide. \n";
    std::fill_n(scaleBuf, bufLen, 0);
    GenArp(key.notes["1st"], 1000, 32, halfMag, scale.buf, 0, false);
    AudioPlayer(scale);
    Sleep(1500);

    std::cout << "arpeggio with a note length of 32/sec, slide. \n";
    std::fill_n(scaleBuf, bufLen, 0);
    GenArp(key.notes["1st"], 1000, 32, halfMag, scale.buf, 0, true);
    AudioPlayer(scale);
    Sleep(1500);

    std::cout << "arpeggio with a note length of 16/sec, no slide. \n";
    std::fill_n(scaleBuf, bufLen, 0);
    GenArp(key.notes["1st"], 1000, 16, halfMag, scale.buf, 0, false);
    AudioPlayer(scale);
    Sleep(1500);

    std::cout << "arpeggio with a note length of 16/sec, slide. \n";
    std::fill_n(scaleBuf, bufLen, 0);
    GenArp(key.notes["1st"], 1000, 16, halfMag, scale.buf, 0, true);
    AudioPlayer(scale);
    Sleep(1500);


    scale.length = 500 * audioSettings.bytesPerMS;

    std::cout << "little song";
    std::fill_n(scaleBuf, bufLen, 0);
    GenArp(key.notes["1st"], 500, 32, halfMag, scale.buf, 0, false);
    AudioPlayer(scale);
    Sleep(500);

    std::fill_n(scaleBuf, bufLen, 0);
    GenArp(key.notes["3rd"], 500, 32, halfMag, scale.buf, 0, false);
    AudioPlayer(scale);
    Sleep(500);

    std::fill_n(scaleBuf, bufLen, 0);
    GenArp(key.notes["1st"], 500, 32, halfMag, scale.buf, 0, false);
    AudioPlayer(scale);
    Sleep(500);

    std::fill_n(scaleBuf, bufLen, 0);
    GenArp(key.notes["3rd"], 500, 32, halfMag, scale.buf, 0, false);
    AudioPlayer(scale);
    Sleep(500);

    std::fill_n(scaleBuf, bufLen, 0);
    GenArp(key.notes["1st"], 500, 16, halfMag, scale.buf, 0, false);
    AudioPlayer(scale);
    Sleep(500);

    std::fill_n(scaleBuf, bufLen, 0);
    GenArp(key.notes["3rd"], 500, 16, halfMag, scale.buf, 0, false);
    AudioPlayer(scale);
    Sleep(500);

    std::fill_n(scaleBuf, bufLen, 0);
    GenArp(key.notes["1st"], 500, 16, halfMag, scale.buf, 0, false);
    AudioPlayer(scale);
    Sleep(500);

    std::fill_n(scaleBuf, bufLen, 0);
    GenArp(key.notes["3rd"], 500, 16, halfMag, scale.buf, 0, false);
    AudioPlayer(scale);
    Sleep(500);

}


void PlayScale()
{
    SetupAudio();

    std::string strInputNoteName;
    float fInputFreq;
    AudioData scale;

    // Buffer for total scale (8 notes) in 8-bit format
    int bufLen = internalAudioBuffer.length;
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
    
    auto key = Scale(Key::Major, fInputFreq);
   
    int noteLen = songSettings.noteLenBytes;

    Sine(key.notes["1st"], songSettings.noteLenMS, halfMag, &scaleBuf[0]);
    Sine(key.notes["2nd"], songSettings.noteLenMS, halfMag, &scaleBuf[noteLen]);
    Sine(key.notes["3rd"], songSettings.noteLenMS, halfMag, &scaleBuf[noteLen * 2]);
    Sine(key.notes["4th"], songSettings.noteLenMS, halfMag, &scaleBuf[noteLen * 3]);
    Sine(key.notes["5th"], songSettings.noteLenMS, halfMag, &scaleBuf[noteLen * 4]);
    Sine(key.notes["6th"], songSettings.noteLenMS, halfMag, &scaleBuf[noteLen * 5]);
    Sine(key.notes["7th"], songSettings.noteLenMS, halfMag, &scaleBuf[noteLen * 6]);
    Sine(key.notes["8th"], songSettings.noteLenMS, halfMag, &scaleBuf[noteLen * 7]);

#ifdef DEBUG_AUDIO
    DumpBuffer(scaleBuf, bufLen, "ScaleSine.txt");
#endif
    
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

// Arpeggio generation
void GenArp(float freq, int arpLengthMS, int NoteLength, int magnitude, Uint8* inBuf, int currPo, bool slide)
{
    Scale key = Scale(songSettings.key, freq);
    Scale key2 = Scale(songSettings.key, key.notes["8th"]);

    int arpNoteLenMS = 1000 / NoteLength; // noteLength is either 64th of a second, 32nd of a second, etc.
    int arpNoteLenBytes = arpNoteLenMS * audioSettings.bytesPerMS;

    int noteCounter = 1;
    //arpLengthMs is total length
    for (int c = 0; c < arpLengthMS * audioSettings.bytesPerMS; c += (arpNoteLenBytes * noteCounter))
    {
        SafeLead(key.notes["1st"], arpNoteLenMS, magnitude, inBuf, currPo + (arpNoteLenBytes * noteCounter));
        noteCounter++;
        if (slide) {
            SlideSquare(key.notes["1st"], key.notes["3rd"], arpNoteLenMS, magnitude, inBuf, currPo + (arpNoteLenBytes * noteCounter));
            noteCounter++;
        }
        SafeLead(key.notes["3rd"], arpNoteLenMS, magnitude, inBuf, currPo + (arpNoteLenBytes * noteCounter));
        noteCounter++;
        if (slide) {
            SlideSquare(key.notes["3rd"], key.notes["5th"], arpNoteLenMS, magnitude, inBuf, currPo + (arpNoteLenBytes * noteCounter));
            noteCounter++;
        }
        SafeLead(key.notes["5th"], arpNoteLenMS, magnitude, inBuf, currPo + (arpNoteLenBytes * noteCounter));
        noteCounter++;
        if (slide) {
            SlideSquare(key.notes["3rd"], key2.notes["1st"], arpNoteLenMS, magnitude, inBuf, currPo + (arpNoteLenBytes * noteCounter));
            noteCounter++;
        }
        SafeLead(key2.notes["1st"], arpNoteLenMS, magnitude, inBuf, currPo + (arpNoteLenBytes * noteCounter));
        noteCounter++;
        if (slide) {
            SlideSquare(key2.notes["1st"], key2.notes["3rd"], arpNoteLenMS, magnitude, inBuf, currPo + (arpNoteLenBytes * noteCounter));
            noteCounter++;
        }
        SafeLead(key2.notes["3rd"], arpNoteLenMS, magnitude, inBuf, currPo + (arpNoteLenBytes * noteCounter));
        noteCounter++;
        if (slide) {
            SlideSquare(key2.notes["3rd"], key2.notes["5th"], arpNoteLenMS, magnitude, inBuf, currPo + (arpNoteLenBytes * noteCounter));
            noteCounter++;
        }
        SafeLead(key2.notes["5th"], arpNoteLenMS, magnitude, inBuf, currPo + (arpNoteLenBytes * noteCounter));
        noteCounter++;
    }
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

void SetupAudio(bool callback)
{
    audioSettings.Init(callback);
    songSettings.Init();
    internalAudioBuffer.Init();
}

int InitSDL()
{
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
    return 0;
}


void ConfigSong(int bpm, char note, int scale, bool lofi)
{
    // scale == 1 then major; scale == 0 then minor
    songSettings.BPM = bpm;
    songSettings.keyNote = note;
    if (scale > 0)
        songSettings.key = Key::Major;
    else
        songSettings.key = Key::Minor;
    
    if (lofi)
    {
        songSettings.loFi = true;
        audioSettings.audSpecWant.freq = 8000;
        audioSettings.audSpecWant.samples = 4096;
    
    } else {
        songSettings.loFi = false;
        audioSettings.audSpecWant.freq = 48000;
        audioSettings.audSpecWant.samples = 32768;
    }
}


void RandomConfig()
{
    songSettings.BPM = ((rand() % 45) * 4) + 60;
    char note = (rand() % 7) + 65;
    songSettings.keyNote = note;
    if (rand() % 2)
        songSettings.key = Key::Major;
    else
        songSettings.key = Key::Minor;
    
    if (rand() % 2)
    {
        songSettings.loFi = true;
        audioSettings.audSpecWant.freq = 8000;
        audioSettings.audSpecWant.samples = 4096;
    
    } else {
        songSettings.loFi = false;
        audioSettings.audSpecWant.freq = 48000;
        audioSettings.audSpecWant.samples = 32768;
    }
}


userSettings GetSongSettings(){
    userSettings us;
    us.BPM = songSettings.BPM;
    us.note = songSettings.keyNote[0];
    if (songSettings.key == Key::Major)
        us.scale = 1;
    else
        us.scale = 0;
    us.lofi = songSettings.loFi;
    return us;
}



Key SwitchKeyMode(Key key)
{
    if (key == Key::Major) {
        return Key::Minor;
    }
    return Key::Major;
};

