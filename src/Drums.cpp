//
//  Drums.cpp
//  test
//
//  Created by Morgan on 28/2/20.
//  Copyright © 2020 Morgan. All rights reserved.
//

#include "MusicGen.h"


void GenDrumBeat(Uint8 *drumBuf)
{
    AudioData kick = songSettings.kickSound;
    AudioData hihat = songSettings.hihatSound;
    AudioData snare = songSettings.snareSound;

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

void TestDrums()
{
    AudioData silenceSec = Silence(1000);
    AudioData jiffy = Silence(100);

    AudioData kick = GiveKick();
    DumpBuffer(kick.buf, kick.length, "Kick.txt");
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

    Uint32 waveLength = samplesPerMS * kickLength * 2;

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
