//
//  AudioAndMusic.cpp
//
//  Created by Morgan on 28/2/20.
//  Copyright © 2020 Morgan. All rights reserved.
//
#ifdef _WIN64
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "MusicGen.h"

struct AudioSettings audioSettings;
struct SongSettings songSettings;
struct InternalAudioBuffer internalAudioBuffer;
std::mt19937 mtRNG;

// Callback function.  SDL will call this function when it wants audio.  Our mission is to fill the buffer.
// len is number of bytes not number of samples requested.
void GenAudioStream(void* userdata, Uint8* stream, int len)
{
    static SDL_Thread* backBufferThread;

#ifdef DEBUG_AUDIO
    std::cout << "\n\n** AUDIO STREAM DEBUG INFO ** \n";
    int noOfSamplesRequested = len / 2;
    std::cout << "No. of bytes requested: " << len << "\nLength of audio requested (ms): " << noOfSamplesRequested / audioSettings.samplesPerMS << "\n";
    std::cout << "No. of samples requested (calced): " << len / sizeof(int16_t) << "\n";
    std::cout << "No. of samples requested 2 (var) : " << noOfSamplesRequested << ".\n";
    std::cout << "Samples per ms: " << audioSettings.samplesPerMS << ".\n";
    std::cout << "ms of audio requested: " << (float)noOfSamplesRequested / audioSettings.samplesPerMS << "\n";
    std::cout << "internalAudioBuffer.backBufLength; " << internalAudioBuffer.backBufferLength << "\n";
    std::cout << "internalAudioBuffer.length; " << internalAudioBuffer.length << "\n";
#endif

    int bytesTillIntBufEnd = internalAudioBuffer.length - internalAudioBuffer.pos;

    if (!audioSettings.inited || !songSettings.inited || !internalAudioBuffer.inited) // Is this being called prematurely?
    {
        std::cout << "Skipping callback as music functionality is not yet ready";
    }
    else if (internalAudioBuffer.pos == -1) // Fill audio buffer on first run.
    {
        //Generate the music stream on the main thread the first time only.
        internalAudioBuffer.pos = 0;
        GenMusic(internalAudioBuffer.buf);

        memcpy(stream, &internalAudioBuffer.buf[internalAudioBuffer.pos], len);
        internalAudioBuffer.pos += len;

        // Fill back buffer
        backBufferThread = SDL_CreateThread(GenMusic, "backBuffer", internalAudioBuffer.backBuf);
        if (NULL == backBufferThread)
            printf("SDL_CreateThread failed: %s\n", SDL_GetError());

    }
    else if (bytesTillIntBufEnd < len) // We are near the end of the internal buffer & need to generate the next section
    {
        // Copy the remaining data from the internal buffer
        memcpy(stream, &internalAudioBuffer.buf[internalAudioBuffer.pos], bytesTillIntBufEnd);
        internalAudioBuffer.pos += bytesTillIntBufEnd;
        // Calculate how much extra data we need
        int extraBytesRequired = len - bytesTillIntBufEnd;

        int randTestChance = mtRNG() % 100;

        // Generate a new drum beat / music track or repeat the old one?
        if (randTestChance < 20)
        {
            internalAudioBuffer.pos = 0;
            std::cout << "\n   > RNJesus wants the previous section to repeat first... \n";
        }
        else
        {
            std::cout << "\n                                                *** Song Settings: *** \n"; // Should probably use printf() but BOO!
            std::cout << "\nBPM: " << songSettings.BPM << "\n";
            std::cout << "Base key: " << songSettings.keyNote << " " << (songSettings.scaleType == ScaleType::Major ? "Major" : "Minor") << "\n";
            std::cout << "Scale in Key: " << songSettings.keyDeg << "\n";
            std::cout << "LoFi: " << (songSettings.loFi == true ? "Yep" : "Nup") << "\n";
            std::cout << "Seed word: " << songSettings.rngSeedString << "\n";

            if (backBufferThread != NULL)
            {
                // Yield thread
                int retVal = NULL;
                // std::cout << "\nWaiting for BG thread...\n"; //DEBUG
                SDL_WaitThread(backBufferThread, &retVal);
                backBufferThread = NULL;

                // Refresh primary buffer
                internalAudioBuffer.InitBuffer();
                if (retVal != NULL)
                {
#ifndef _WIN64
                    memmove(internalAudioBuffer.buf, internalAudioBuffer.backBuf, std::min(internalAudioBuffer.length, internalAudioBuffer.backBufferLength));
#else
                    memmove(internalAudioBuffer.buf, internalAudioBuffer.backBuf, min(internalAudioBuffer.length, internalAudioBuffer.backBufferLength));
#endif
                    internalAudioBuffer.pos = 0;
                }
                else {
                    std::cout << "***ERROR*** Something happened with the BG thread!!! \n";
                }

                RNJesusSongSettings(randTestChance);

                // Launch new thread to generate music to backBuffer
                // std::cout << "Launching new BG thread to generate next section. \n";
                backBufferThread = SDL_CreateThread(GenMusic, "backBuffer", internalAudioBuffer.backBuf);
                if (NULL == backBufferThread)
                    printf("SDL_CreateThread failed: %s\n", SDL_GetError());
            }
            else
            {
                std::cout << "***ERROR*** Something happened with the BG thread!!! \n";
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

#ifdef DUMP_BUFFERS
    DumpBuffer(stream, len, "ReqBuf.txt");
#endif
}

// Evaluate switching musical scales (within key) or BPM between thread generations
void RNJesusSongSettings(int randTestChance)
{
    
    // switch scales to different scale in same key - 2% chance
    // Edit: This is wrong from a music theory perspective.
    if (randTestChance > 94 && randTestChance < 97 && internalAudioBuffer.backBufferLength == internalAudioBuffer.length)
    {
        SwitchScaleDegree();
    }

    // Switch to random key - 1% chance
    if (randTestChance > 20 && randTestChance < 22 && internalAudioBuffer.backBufferLength == internalAudioBuffer.length)
    {
        char note = (mtRNG() % 7) + 65;
        songSettings.keyNote = note;
        if (mtRNG() % 2)
            songSettings.scaleType = ScaleType::Major;
        else
            songSettings.scaleType = ScaleType::Minor;

        songSettings.bassBaseScaleFreq = Notes.getNoteFreq(songSettings.keyNote + "1");
        songSettings.leadBaseScaleFreq = Notes.getNoteFreq(songSettings.keyNote + "3");

        std::cout << "\n   > RNJesus wants to change the song key to: " << songSettings.keyNote << " " << (songSettings.scaleType == ScaleType::Major ? "Major" : "Minor") << "\n";
    }

    // switch bpm? 4% chance or 10% is slower than 100 BPM
    if (((randTestChance > 35 && randTestChance < 40) ||
        (songSettings.BPM <= 100 && randTestChance % 10 == 0)) && // Encourage slower songs to speed up
        internalAudioBuffer.backBufferLength == internalAudioBuffer.length)
    {
        songSettings.BPM = ((mtRNG() % 55) * 4) + 60; // range of 60 - 260 BPM

        if (songSettings.BPM < 100) // bias for faster BPM
            songSettings.BPM = ((mtRNG() % 55) * 4) + 60; // range of 60 - 260 BPM

        std::cout << "\n   > RNJesus wants to change the BPM to: " << songSettings.BPM << "\n";
        // reinit
        songSettings.Init();
        internalAudioBuffer.InitBackBuffer();
    }
}


int GenMusic(void* ptr)
{
    std::cout << "\n                                              *** Upcoming section: " << songSettings.sectionCount <<" *** \n\n"; // Should probably use printf() but BOO!
    songSettings.sectionCount++;

    Uint8* inBuf = (Uint8*)ptr;

    // Clear internal buffer
    std::fill_n(inBuf, internalAudioBuffer.backBufferLength, 0);

    // Gen Drums
    Uint8* drumBuf = new Uint8[internalAudioBuffer.backBufferLength]();
    std::fill_n(drumBuf, internalAudioBuffer.backBufferLength, 0);
    if (songSettings.genDrums)
        GenDrumBeat(drumBuf, internalAudioBuffer.backBufferLength);

    // Gen Bass
    Uint8* bassBuf = new Uint8[internalAudioBuffer.backBufferLength]();
    if (songSettings.genBass)
        GenBassTrack(bassBuf, internalAudioBuffer.backBufferLength);

    // Gen Lead
    Uint8* leadBuf = new Uint8[internalAudioBuffer.backBufferLength]();
    if (songSettings.genLead)
        GenLeadTrack(leadBuf, internalAudioBuffer.backBufferLength);

    // Mix in drums
    if (songSettings.genDrums)
        SDL_MixAudioFormat(inBuf, drumBuf, sampleFmt, internalAudioBuffer.backBufferLength, SDL_MIX_MAXVOLUME);

    // Mix in bass
    if (songSettings.genBass)
        SDL_MixAudioFormat(inBuf, bassBuf, sampleFmt, internalAudioBuffer.backBufferLength, SDL_MIX_MAXVOLUME);

#ifndef DISABLE_REVERB
    Uint8* leadOutBuf = new Uint8[internalAudioBuffer.backBufferLength]();
    short* leadOutBuf16 = new short[internalAudioBuffer.backBufferLength]();
    short* leadBuf16 = new short[internalAudioBuffer.backBufferLength]();
#endif

    // Mix in lead
    if (songSettings.genLead)
    {
#ifndef DISABLE_REVERB
        // *** Mix in reverb ***
        // convert to 16 bit buf, apply reverb and convert back to 8 bit buf and mix into lead track. Better options?
        memcpy(&leadBuf16[0], &leadBuf[0], internalAudioBuffer.backBufferLength);
        Reverb(leadBuf16, leadOutBuf16, internalAudioBuffer.backBufferLength);
        c16to8(leadOutBuf16, internalAudioBuffer.backBufferLength / 2, leadOutBuf);
        SDL_MixAudioFormat(leadBuf, leadOutBuf, sampleFmt, internalAudioBuffer.backBufferLength, SDL_MIX_MAXVOLUME);
#endif
        SDL_MixAudioFormat(inBuf, leadBuf, sampleFmt, internalAudioBuffer.backBufferLength, SDL_MIX_MAXVOLUME);
    }

#ifdef DUMP_PRIMARY_BUFFERS
    DumpBuffer(drumBuf, internalAudioBuffer.length, "DrumBuffer" + std::to_string(songSettings.sectionCount) + ".txt");
    DumpBuffer(bassBuf, internalAudioBuffer.length, "BassBuffer" + std::to_string(songSettings.sectionCount) + ".txt");
    DumpBuffer(leadBuf, internalAudioBuffer.length, "LeadBuffer" + std::to_string(songSettings.sectionCount) + ".txt");
    DumpBuffer(inBuf, internalAudioBuffer.length, "FullBuffer" + std::to_string(songSettings.sectionCount) + ".txt");
    std::cout << "Dumping Buffers!\n";
#endif

    delete[] drumBuf;
    delete[] bassBuf;
    delete[] leadBuf;
    // std::cout << "\nThread finished processing!\n";

#ifndef DISABLE_REVERB
    delete[] leadOutBuf;
    delete[] leadOutBuf16;
    delete[] leadBuf16;
#endif
    return 1;
}


// Key function.  Must be called before any audio is played.  callback determines whether music is streamed.
void SetupAudio(bool callback)
{
    audioSettings.Init(callback);
    songSettings.Init();
    // songSettings.ReinitDrums();
    internalAudioBuffer.Init();
    songSettings.sectionCount = 1;
    SDL_PauseAudioDevice(audioSettings.device, 0); // Unpause audio device

    std::cout << " --- CURRENT SETTINGS ---\n\n";
    std::cout << " BPM: " << songSettings.BPM << "\n";
    std::cout << " Key/Scale: " << songSettings.keyNote << " " << (songSettings.scaleType == ScaleType::Major ? "Major" : "Minor") << "\n";
    std::cout << " LoFi: " << (songSettings.loFi == true ? "Yep" : "Nup") << "\n";
    std::cout << " Seed word: " << songSettings.rngSeedString << " Seed number: " << songSettings.rngSeed << "\n";
}


void RandomConfig()
{
    songSettings.rngSeedString = RandomWordFromWordList();
    SeedConfig();
}

void SeedConfig()
{
    // reset instruments
    songSettings.leadSine = false;
    songSettings.leadSawtooth = true;
    songSettings.leadSquare = false;

    // Seed RNG
    songSettings.rngSeed = WordToNumber(songSettings.rngSeedString);
    mtRNG.seed(songSettings.rngSeed);

    songSettings.BPM = ((mtRNG() % 45) * 4) + 60;
    char note = (mtRNG() % 7) + 65;
    songSettings.keyNote = note;
    songSettings.bassBaseScaleFreq = Notes.getNoteFreq(std::string(1, note) + "1");
    songSettings.leadBaseScaleFreq = Notes.getNoteFreq(std::string(1, note) + "3");

    if (mtRNG() % 2) {
        songSettings.scaleType = ScaleType::Major;
        songSettings.keyType = ScaleType::Major;
    }
    else {
        songSettings.scaleType = ScaleType::Minor;
        songSettings.keyType = ScaleType::Minor;

    }
}


ScaleType OppositeKeyMode(ScaleType key)
{
    if (key == ScaleType::Major) {
        return ScaleType::Minor;
    }
    return ScaleType::Major;
};


// Pick a new scale in existing key with some biases towards particular scales.
// Edit: I don't think this makes sense musically
void SwitchScaleDegree()
{

    int newKeyDegree = mtRNG() % 7 + 1;

    // Get frequency for bass instrument
    std::pair<float, ScaleType> newBassFreqTypePair = GiveKeyScale(Notes.getNoteFreq(songSettings.keyNote + "1"), songSettings.keyType, newKeyDegree);
    songSettings.scaleType = newBassFreqTypePair.second;
    songSettings.bassBaseScaleFreq = newBassFreqTypePair.first;

    // Get frequency for lead instrument
    std::pair<float, ScaleType> newLeadFreqTypePair = GiveKeyScale(Notes.getNoteFreq(songSettings.keyNote + "3"), songSettings.keyType, newKeyDegree);
    songSettings.leadBaseScaleFreq = newLeadFreqTypePair.first;

    // User interface stuff
    songSettings.keyDeg = std::to_string(newKeyDegree);
    std::cout << "\n   > RNJesus wants to change the scale to this degree of the song's key: " << newKeyDegree << "\n"; // " << which has a frequency of " << newLeadFreqTypePair.first << "hz for the lead instrument\n";

}


// Returns the frequency & scale type of a particular degree in an existing scale.
// Edit: This doesn't actually make sense musically as I was conflating chords in a key and scale degrees of a key at the time.
// Further edit: And perhaps that is why it doesn't sound great.
std::pair<float, ScaleType> GiveKeyScale(float baseFreq, ScaleType keyType, int newDegree)
{
    // Return the input note and key type if the input doesn't make sense.
    if (newDegree == 1 || newDegree >= 8)
    {
        return std::pair<float, ScaleType>(baseFreq, keyType);
    }

    // Return the input note and key type if the new scale will be diminished as I don't have a diminished generator
    if (keyType == ScaleType::Major && newDegree == 7)
    {
        std::cout << "\n   > RNJesus can't change to 7th degree of Major scale - no diminished scale gen \n";
        return std::pair<float, ScaleType>(baseFreq, keyType);
    }

    // Return the input note and key type if the new scale will be diminished as I don't have a diminished generator
    if (keyType == ScaleType::Minor && newDegree == 2)
    {
        std::cout << "\n   > RNJesus can't change to 2nd degree of Minor scale - no diminished scale gen \n";
        return std::pair<float, ScaleType>(baseFreq, keyType);
    }


    // Construct key scale
    Scale key(keyType, baseFreq); 

    ScaleType newKeyType;
    if (newDegree == 4 || newDegree == 5) // If the new scale degree is the fourth of fifth, return the same key/scale type.
        newKeyType = keyType;
    else
        newKeyType = OppositeKeyMode(keyType);

    newDegree = newDegree - 1;
    float freqOfScaleDegree = key.freqs[newDegree];

    return std::pair<float, ScaleType>(freqOfScaleDegree, newKeyType);
}

// Arpeggio generation
void GenArp(float freq, int arpLengthMS, int NoteLength, int magnitude, Uint8* inBuf, int currPo, bool slide)
{
    Scale key = Scale(songSettings.scaleType, freq);
    Scale key2 = Scale(songSettings.scaleType, key.notes["8th"]);

    int arpNoteLenMS = 1000 / NoteLength; // noteLength is either 64th of a second, 32nd of a second, etc.
    int arpNoteLenBytes = arpNoteLenMS * audioSettings.bytesPerMS;

    int noteCounter = 0;
    //arpLengthMs is total length
    for (int c = 0; c < arpLengthMS * audioSettings.bytesPerMS; c += arpNoteLenBytes)
    {
        if (noteCounter > 8)
            noteCounter = 0;

        if (!slide)
        {
            if (noteCounter == 0)
                SafeLead(key.notes["1st"], arpNoteLenMS, magnitude, inBuf, c);
            if (noteCounter == 1)
                SafeLead(key.notes["3rd"], arpNoteLenMS, magnitude, inBuf, c);
            if (noteCounter == 2)
                SafeLead(key.notes["5th"], arpNoteLenMS, magnitude, inBuf, c);
            if (noteCounter == 3)
                SafeLead(key.notes["7th"], arpNoteLenMS, magnitude, inBuf, c);
            if (noteCounter == 4)
                SafeLead(key2.notes["1st"], arpNoteLenMS, magnitude, inBuf, c);
            if (noteCounter == 5)
                SafeLead(key2.notes["3rd"], arpNoteLenMS, magnitude, inBuf, c);
            if (noteCounter == 6)
                SafeLead(key2.notes["5th"], arpNoteLenMS, magnitude, inBuf, c);
            if (noteCounter == 7)
                SafeLead(key2.notes["7th"], arpNoteLenMS, magnitude, inBuf, c);
        }
        else
        {
            if (noteCounter == 0)
                SlideSquare(key.notes["1st"], key.notes["3rd"], arpNoteLenMS, magnitude, inBuf, c);
            if (noteCounter == 1)
                SlideSquare(key.notes["3rd"], key.notes["5th"], arpNoteLenMS, magnitude, inBuf, c);
            if (noteCounter == 2)
                SlideSquare(key.notes["5th"], key.notes["7th"], arpNoteLenMS, magnitude, inBuf, c);
            if (noteCounter == 3)
                SlideSquare(key.notes["7th"], key2.notes["1st"], arpNoteLenMS, magnitude, inBuf, c);
            if (noteCounter == 4)
                SlideSquare(key2.notes["1st"], key2.notes["3rd"], arpNoteLenMS, magnitude, inBuf, c);
            if (noteCounter == 5)
                SlideSquare(key2.notes["3rd"], key2.notes["5th"], arpNoteLenMS, magnitude, inBuf, c);
            if (noteCounter == 6)
                SlideSquare(key2.notes["5th"], key2.notes["7th"], arpNoteLenMS, magnitude, inBuf, c);
            if (noteCounter == 7)
                SlideSquare(key2.notes["7th"], key2.notes["8th"], arpNoteLenMS, magnitude, inBuf, c);
        }
        noteCounter++;
    }
}

// Play music from a AudioData obj (buffer + length)
void AudioPlayer(AudioData audioData)
{
    //std::cout << "Playing...\n";

    int success = SDL_QueueAudio(audioSettings.device, audioData.buf, audioData.length);

    if (success != 0)
        std::cout << "\nError outputting to device!!\n";
}

// Unused function at this point in time
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


// Used when built as lib
userSettings GetSongSettings() {
    userSettings us;
    us.BPM = songSettings.BPM;
    us.note = songSettings.keyNote[0];
    if (songSettings.scaleType == ScaleType::Major)
        us.scale = 1;
    else
        us.scale = 0;
    us.lofi = songSettings.loFi;
    return us;
}

void ConfigSong(int bpm, char note, int scale, bool lofi)
{
    // scale == 1 then major; scale == 0 then minor
    songSettings.BPM = bpm;
    songSettings.keyNote = note;
    if (scale > 0)
        songSettings.scaleType = ScaleType::Major;
    else
        songSettings.scaleType = ScaleType::Minor;

    if (lofi)
    {
        songSettings.loFi = true;
        audioSettings.audSpecWant.freq = 8000;
        //audioSettings.audSpecWant.samples = 4096;
    }
    else {
        songSettings.loFi = false;
        audioSettings.audSpecWant.freq = 48000;
        //audioSettings.audSpecWant.samples = 32768;
    }
}

#ifdef ENABLE_DEBUG_FUNCTIONS
void TestGiveScaleKey()
{

    Scale CMaj(ScaleType::Major, Notes.getNoteFreq("C4"));
    std::pair<float, ScaleType> GMajMaybe = GiveKeyScale(CMaj.keyFreq, ScaleType::Major, 5);
    std::pair<float, ScaleType> GMajDef(Notes.getNoteFreq("G4"), ScaleType::Major);

    std::cout << "GMajMaybe Freq: " << GMajMaybe.first << " GMajDef Freq: " << GMajDef.first << "\n";
    std::cout << "GMajMaybe Type: " << GMajMaybe.second << " GMajDef Type: " << GMajDef.second << "\n";
    SDL_assert(GMajDef.first == round(GMajMaybe.first));
    SDL_assert(GMajDef.second == GMajMaybe.second);

    std::pair<float, ScaleType> EminMaybe = GiveKeyScale(CMaj.keyFreq, ScaleType::Major, 3);
    std::pair<float, ScaleType> EminDef(Notes.getNoteFreq("E4"), ScaleType::Minor);

    std::cout << "EminMaybe Freq: " << EminMaybe.first << " EminDef Freq: " << EminDef.first << "\n";
    std::cout << "EminMaybe Type: " << EminMaybe.second << " EminDef Type: " << EminDef.second << "\n";
    SDL_assert(EminDef.first == round(EminMaybe.first));
    SDL_assert(EminDef.second == EminMaybe.second);

    std::cout << "Test passed! \n";
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

    auto key = Scale(ScaleType::Major, fInputFreq);

    int noteLen = songSettings.noteLenBytes;

    scale.buf = scaleBuf;
    scale.length = bufLen;

    std::cout << "arpeggio with a note length of 64/sec, no slide. \n";
    std::fill_n(scaleBuf, bufLen, 0);
    GenArp(key.notes["1st"], 1000, 64, halfMag, scale.buf, 0, false);
    AudioPlayer(scale);
#ifndef _WIN64
    usleep(1500);
#else
    Sleep(1500);
#endif

    std::cout << "arpeggio with a note length of 64/sec, slide. \n";
    std::fill_n(scaleBuf, bufLen, 0);
    GenArp(key.notes["1st"], 1000, 64, halfMag, scale.buf, 0, true);
    AudioPlayer(scale);
#ifndef _WIN64
    usleep(1500);
#else
    Sleep(1500);
#endif

    std::cout << "arpeggio with a note length of 32/sec, no slide. \n";
    std::fill_n(scaleBuf, bufLen, 0);
    GenArp(key.notes["1st"], 1000, 32, halfMag, scale.buf, 0, false);
    AudioPlayer(scale);
#ifndef _WIN64
    usleep(1500);
#else
    Sleep(1500);
#endif

    std::cout << "arpeggio with a note length of 32/sec, slide. \n";
    std::fill_n(scaleBuf, bufLen, 0);
    GenArp(key.notes["1st"], 1000, 32, halfMag, scale.buf, 0, true);
    AudioPlayer(scale);
#ifndef _WIN64
    usleep(1500);
#else
    Sleep(1500);
#endif

    std::cout << "arpeggio with a note length of 16/sec, no slide. \n";
    std::fill_n(scaleBuf, bufLen, 0);
    GenArp(key.notes["1st"], 1000, 16, halfMag, scale.buf, 0, false);
    AudioPlayer(scale);
#ifndef _WIN64
    usleep(1500);
#else
    Sleep(1500);
#endif

    std::cout << "arpeggio with a note length of 16/sec, slide. \n";
    std::fill_n(scaleBuf, bufLen, 0);
    GenArp(key.notes["1st"], 1000, 16, halfMag, scale.buf, 0, true);
    AudioPlayer(scale);
#ifndef _WIN64
    usleep(1500);
#else
    Sleep(1500);
#endif


    scale.length = 500 * audioSettings.bytesPerMS;

    std::cout << "little song";
    std::fill_n(scaleBuf, bufLen, 0);
    GenArp(key.notes["1st"], 500, 32, halfMag, scale.buf, 0, false);
    AudioPlayer(scale);
#ifndef _WIN64
    usleep(500);
#else
    Sleep(500);
#endif

    std::fill_n(scaleBuf, bufLen, 0);
    GenArp(key.notes["3rd"], 500, 32, halfMag, scale.buf, 0, false);
    AudioPlayer(scale);
#ifndef _WIN64
    usleep(500);
#else
    Sleep(500);
#endif

    std::fill_n(scaleBuf, bufLen, 0);
    GenArp(key.notes["1st"], 500, 32, halfMag, scale.buf, 0, false);
    AudioPlayer(scale);
#ifndef _WIN64
    usleep(500);
#else
    Sleep(500);
#endif

    std::fill_n(scaleBuf, bufLen, 0);
    GenArp(key.notes["3rd"], 500, 32, halfMag, scale.buf, 0, false);
    AudioPlayer(scale);
#ifndef _WIN64
    usleep(500);
#else
    Sleep(500);
#endif

    std::fill_n(scaleBuf, bufLen, 0);
    GenArp(key.notes["1st"], 500, 16, halfMag, scale.buf, 0, false);
    AudioPlayer(scale);
#ifndef _WIN64
    usleep(500);
#else
    Sleep(500);
#endif

    std::fill_n(scaleBuf, bufLen, 0);
    GenArp(key.notes["3rd"], 500, 16, halfMag, scale.buf, 0, false);
    AudioPlayer(scale);
#ifndef _WIN64
    usleep(500);
#else
    Sleep(500);
#endif

    std::fill_n(scaleBuf, bufLen, 0);
    GenArp(key.notes["1st"], 500, 16, halfMag, scale.buf, 0, false);
    AudioPlayer(scale);
#ifndef _WIN64
    usleep(500);
#else
    Sleep(500);
#endif

    std::fill_n(scaleBuf, bufLen, 0);
    GenArp(key.notes["3rd"], 500, 16, halfMag, scale.buf, 0, false);
    AudioPlayer(scale);
#ifndef _WIN64
    usleep(500);
#else
    Sleep(500);
#endif

}


void PlayMajorScale()
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

    auto key = Scale(ScaleType::Major, fInputFreq);

    int noteLen = songSettings.noteLenBytes;

    Sine(key.notes["1st"], songSettings.noteLenMS, halfMag, &scaleBuf[0]);
    Sine(key.notes["2nd"], songSettings.noteLenMS, halfMag, &scaleBuf[noteLen]);
    Sine(key.notes["3rd"], songSettings.noteLenMS, halfMag, &scaleBuf[noteLen * 2]);
    Sine(key.notes["4th"], songSettings.noteLenMS, halfMag, &scaleBuf[noteLen * 3]);
    Sine(key.notes["5th"], songSettings.noteLenMS, halfMag, &scaleBuf[noteLen * 4]);
    Sine(key.notes["6th"], songSettings.noteLenMS, halfMag, &scaleBuf[noteLen * 5]);
    Sine(key.notes["7th"], songSettings.noteLenMS, halfMag, &scaleBuf[noteLen * 6]);
    Sine(key.notes["8th"], songSettings.noteLenMS, halfMag, &scaleBuf[noteLen * 7]);

#ifdef DUMP_BUFFERS
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



void PlayMinorScale()
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

    auto key = Scale(ScaleType::Minor, fInputFreq);

    int noteLen = songSettings.noteLenBytes;

    Sine(key.notes["1st"], songSettings.noteLenMS, halfMag, &scaleBuf[0]);
    Sine(key.notes["2nd"], songSettings.noteLenMS, halfMag, &scaleBuf[noteLen]);
    Sine(key.notes["3rd"], songSettings.noteLenMS, halfMag, &scaleBuf[noteLen * 2]);
    Sine(key.notes["4th"], songSettings.noteLenMS, halfMag, &scaleBuf[noteLen * 3]);
    Sine(key.notes["5th"], songSettings.noteLenMS, halfMag, &scaleBuf[noteLen * 4]);
    Sine(key.notes["6th"], songSettings.noteLenMS, halfMag, &scaleBuf[noteLen * 5]);
    Sine(key.notes["7th"], songSettings.noteLenMS, halfMag, &scaleBuf[noteLen * 6]);
    Sine(key.notes["8th"], songSettings.noteLenMS, halfMag, &scaleBuf[noteLen * 7]);

#ifdef DUMP_BUFFERS
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

#endif