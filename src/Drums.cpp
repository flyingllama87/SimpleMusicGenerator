//
//  Drums.cpp
//  test
//
//  Created by Morgan on 28/2/20.
//  Copyright © 2020 Morgan. All rights reserved.
//

#include "MusicGen.h"
#include <Windows.h>

void GenDrumBeat(Uint8 *drumBuf)
{
    LARGE_INTEGER cicles;
    QueryPerformanceCounter(&cicles);
    std::srand(cicles.QuadPart);

    AudioData kick = songSettings.kickSound;
    AudioData hihat = songSettings.hihatSound;
    AudioData snare = songSettings.snareSound;

    int beatCount = 1;
    int barCount = 1;

	int pickRandDrumPattern = std::rand() % 7;
    std::cout << "Playing drum pattern: " << pickRandDrumPattern << "\n";

#ifdef DEBUG_AUDIO
    std::cout << "internalAudioBuffer.length: " << internalAudioBuffer.length << "\n";
	std::cout << "songSettings.qtrNoteLenBytes: " << songSettings.qtrNoteLenBytes << "\n";
	std::cout << "songSettings.qtrNoteLenMS: " << songSettings.qtrNoteLenMS << "\n";
	std::cout << "songSettings.halfNoteLenMS: " << songSettings.halfNoteLenMS << "\n";
	std::cout << "songSettings.noteLenMS: " << songSettings.noteLenMS << "\n";
#endif

    switch (pickRandDrumPattern) {
    case 0: // kick x2, hat x2, snare x2, hat x2
        for (int c = 0; c < internalAudioBuffer.length; c += (songSettings.halfNoteLenBytes))
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
        for (int c = 0; c < internalAudioBuffer.length; c += songSettings.noteLenBytes)
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
        for (int c = 0; c < internalAudioBuffer.length; c += songSettings.noteLenBytes)
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

        for (int c = 0; c < internalAudioBuffer.length; c += (songSettings.halfNoteLenBytes))
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
        for (int c = 0; c < internalAudioBuffer.length; c += songSettings.noteLenBytes)
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
        for (int c = 0; c < internalAudioBuffer.length; c += songSettings.noteLenBytes)
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

        for (int c = 0; c < internalAudioBuffer.length; c += songSettings.qtrNoteLenBytes)
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

#ifdef DEBUG_AUDIO
    std::cout << "Final barCount: " << barCount << "\n";
	std::cout << "Final beatCount: " << beatCount << "\n";
    DumpBuffer(drumBuf, internalAudioBuffer.length, "drumBuffer,txt");
#endif

}

void TestDrums()
{
    SetupAudio();
    AudioData silenceSec = Silence(1000);
    AudioData jiffy = Silence(100);

    AudioData kick = GiveKick();
#ifdef DEBUG_AUDIO
    DumpBuffer(kick.buf, kick.length, "kick.txt");
#endif
    AudioPlayer(kick);
    AudioPlayer(jiffy);
    AudioPlayer(kick);

    AudioPlayer(silenceSec);

    AudioData hihat = GiveHihat();
    AudioPlayer(hihat);
    AudioPlayer(jiffy);
    AudioPlayer(hihat);
#ifdef DEBUG_AUDIO
    DumpBuffer(hihat.buf, hihat.length, "hihat.txt");
#endif
    
    AudioPlayer(silenceSec);

    AudioData snare = GiveSnare();
    AudioPlayer(snare);
    AudioPlayer(jiffy);
    AudioPlayer(snare);

#ifdef DEBUG_AUDIO
    DumpBuffer(snare.buf, snare.length, "snare.txt");
#endif
}

AudioData GiveKick()
{
    AudioData returnAD;

    // 200ms kick
    Uint16 kickLength = 200;

    Uint32 waveLength = audioSettings.samplesPerMS * kickLength * 2;

    // declare buffers
    Uint8 *sineBuffer = new Uint8[waveLength];
    Uint8 *waveBuffer = new Uint8[waveLength];

    // Noise
    Noise(kickLength, true, waveBuffer, qtrMag);

    // SineWave
#ifdef _WIN32 || _WIN64
    Sine(60, kickLength, halfMag, sineBuffer);
#else // Crappy speakers on laptops / mobile devices so need a higher freq kick
    Sine(105, kickLength, halfMag, sineBuffer);
#endif

    FadeOut(waveBuffer, waveLength);
    FadeOut(sineBuffer, waveLength);

    // Mix tracks
    SDL_MixAudioFormat(waveBuffer, sineBuffer, sampleFmt, waveLength, SDL_MIX_MAXVOLUME);

    
    returnAD.buf = waveBuffer;
    returnAD.length = waveLength;

    return returnAD;
}


AudioData GiveHihat()
{
    AudioData returnAD;

    // 100ms hihat
    Uint16 hatLength = 100;


    Uint32 waveLength = audioSettings.samplesPerMS * hatLength * 2;
    Uint8* waveBuffer = new Uint8[waveLength];

    // Noise
    Noise(hatLength, false, waveBuffer, qtrMag);

    //Fade
    FadeOut(waveBuffer, waveLength);

    returnAD.buf = waveBuffer;
    returnAD.length = waveLength;

    return returnAD;
}

AudioData GiveSnare()
{
    AudioData returnAD;

    // 250ms snare
    Uint32 snareLength = 250;
    Uint32 const waveLength = audioSettings.samplesPerMS * snareLength * 2;

    // Initialise
    Uint8* waveBuffer = new Uint8[waveLength];
    Uint8* squareBuffer = new Uint8[waveLength];

    // Noise
    Square(120, snareLength / 2, qtrMag / 4, squareBuffer);
    Noise(snareLength, true, waveBuffer, qtrMag);

    FadeOut(waveBuffer, waveLength);
    FadeOut(squareBuffer, waveLength);

    // Mix tracks
    SDL_MixAudioFormat(waveBuffer, squareBuffer, sampleFmt, waveLength, SDL_MIX_MAXVOLUME);

    returnAD.buf = waveBuffer;
    returnAD.length = waveLength;

    return returnAD;
}
