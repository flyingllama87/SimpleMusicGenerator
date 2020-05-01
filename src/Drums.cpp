//
//  Drums.cpp
//
//  Created by Morgan on 28/2/20.
//  Copyright © 2020 Morgan. All rights reserved.
//

#include "MusicGen.h"

#ifdef _win64
#include <Windows.h>
#endif

void GenDrumBeat(Uint8 *drumBuf, int drumBufLength)
{
    AudioData kick = songSettings.kickSound;
    AudioData hihat = songSettings.hihatSound;
    AudioData snare = songSettings.snareSound;
    AudioData kickHat = songSettings.kickHatSound;;
    AudioData snareHat = songSettings.snareHatSound;;

    int beatCount = 1;
    int barCount = 1;

    // Select drum pattern
    int pickRandDrumPattern;
    if (mtRNG() % 2 == 0)
    {
        std::cout << "   > RNJesus wants the drums to play the same pattern as last time... \n";
        pickRandDrumPattern = songSettings.prevPatternDrums;
    }
    else
    {
        pickRandDrumPattern = mtRNG() % 13;

        if (pickRandDrumPattern == 0 || pickRandDrumPattern == 2) // Discourage the use of these patterns by picking again.
            pickRandDrumPattern = mtRNG() % 13;

        if ((pickRandDrumPattern == 7 || pickRandDrumPattern == 5) && songSettings.BPM < 90)
            pickRandDrumPattern = mtRNG() % 13;
    }
    songSettings.prevPatternDrums = pickRandDrumPattern;

    //Testing Code
   /* if (mtRNG() % 2 == 0)
        pickRandDrumPattern = 6;
    else
        pickRandDrumPattern = 3;*/
    //Testing Code
    
#ifdef DEBUG_DRUMS
    std::cout << "drumBufLength: " << drumBufLength << "\n";
	std::cout << "songSettings.qtrNoteLenBytes: " << songSettings.qtrNoteLenBytes << "\n";
	std::cout << "songSettings.qtrNoteLenMS: " << songSettings.qtrNoteLenMS << "\n";
	std::cout << "songSettings.halfNoteLenMS: " << songSettings.halfNoteLenMS << "\n";
	std::cout << "songSettings.noteLenMS: " << songSettings.noteLenMS << "\n";
#endif

    switch (pickRandDrumPattern) {
    case 0:
        std::cout << "Playing drum pattern: " << pickRandDrumPattern << " - kick x2, hat x2, snare x2, hat x2 \n";
        for (int c = 0; c < drumBufLength; c += (songSettings.halfNoteLenBytes))
        {
            if (beatCount == 1 || beatCount == 2)
                SafeMemCopy(drumBuf, kick.buf, kick.length, c, drumBufLength);
            else if (beatCount == 3 || beatCount == 4 || beatCount == 7)
                SafeMemCopy(drumBuf, hihat.buf, hihat.length, c, drumBufLength);
            else if (beatCount == 5 || beatCount == 6)
                SafeMemCopy(drumBuf, snare.buf, snare.length, c, drumBufLength);
            else
            {
                SafeMemCopy(drumBuf, hihat.buf, hihat.length, c, drumBufLength);
                beatCount = 0;
                barCount++;
            }
            beatCount++;
        }
        break;
    case 1: 
        std::cout << "Playing drum pattern: " << pickRandDrumPattern << " - Classic kick, hat, snare, hat \n";

        for (int c = 0; c < drumBufLength; c += songSettings.noteLenBytes)
        {
            if (beatCount == 1)
                SafeMemCopy(drumBuf, kick.buf, kick.length, c, drumBufLength);
            else if (beatCount == 2)
                SafeMemCopy(drumBuf, hihat.buf, hihat.length, c, drumBufLength);
            else if (beatCount == 3)
                SafeMemCopy(drumBuf, snare.buf, snare.length, c, drumBufLength);
            else if (beatCount == 4)
            {
                SafeMemCopy(drumBuf, hihat.buf, hihat.length, c, drumBufLength);
                beatCount = 0;
                barCount++;
            }
            beatCount++;
        }
        break;
    case 2: // hip-hop slow & fast
            
        if (songSettings.BPM <= 120)
        {
            std::cout << "Playing drum pattern: " << pickRandDrumPattern << " - hip-hop slow \n";

            for (int c = 0; c < drumBufLength; c += songSettings.qtrNoteLenBytes)
            {
                if (beatCount == 1 ||
                    beatCount == 8 ||
                    beatCount == 9 ||
                    beatCount == 11)
                    SafeMemCopy(drumBuf, kick.buf, kick.length, c, drumBufLength);
                
                if (beatCount == 5 ||
                    beatCount == 13)
                    SafeMemCopy(drumBuf, snare.buf, snare.length, c, drumBufLength);
                
                if (false)
                    SafeMemCopy(drumBuf, hihat.buf, hihat.length, c, drumBufLength);

                if (beatCount == 16)
                {
                    beatCount = 0;
                    barCount++;
                }
                beatCount++;
            }
        } else // fast / over 120 BPM
        {
            std::cout << "Playing drum pattern: " << pickRandDrumPattern << " - hip-hop fast \n";

            for (int c = 0; c < drumBufLength; c += songSettings.qtrNoteLenBytes)
            {
                
                if (beatCount == 1 ||
                    beatCount == 3 ||
                    beatCount == 5 ||
                    beatCount == 13
                    )
                    SafeMemCopy(drumBuf, kickHat.buf, kickHat.length, c, drumBufLength);
                
                if (beatCount == 7 ||
                    beatCount == 11 ||
                    beatCount == 15)
                    SafeMemCopy(drumBuf, hihat.buf, hihat.length, c, drumBufLength);
                
                if (beatCount == 9)
                    SafeMemCopy(drumBuf, snareHat.buf, snare.length, c, drumBufLength);

                if (beatCount == 16)
                {
                    beatCount = 0;
                    barCount++;
                }
                beatCount++;
            }
        }
            
        break;
    case 3:
    {
        std::cout << "Playing drum pattern: " << pickRandDrumPattern << " - Kick x2 & snare with hi-hat every half beat \n";

        for (int c = 0; c < drumBufLength; c += (songSettings.halfNoteLenBytes))
        {
            if (beatCount == 1)
                SafeMemCopy(drumBuf, kickHat.buf, kickHat.length, c, drumBufLength);
            else if (beatCount == 8)
            {
                SafeMemCopy(drumBuf, hihat.buf, hihat.length, c, drumBufLength);
                beatCount = 0;
                barCount++;
            }
            else if (beatCount == 5)
                SafeMemCopy(drumBuf, snareHat.buf, snareHat.length, c, drumBufLength);
            else
                SafeMemCopy(drumBuf, hihat.buf, hihat.length, c, drumBufLength);
            beatCount++;
        }
        break;
    }
    case 4: // Kick & snare, 2 beats each
        std::cout << "Playing drum pattern: " << pickRandDrumPattern << " - Kick & snare (repeat), 2 beats each \n";

        for (int c = 0; c < drumBufLength; c += songSettings.noteLenBytes)
        {
            if (beatCount == 1)
                SafeMemCopy(drumBuf, kick.buf, kick.length, c, drumBufLength);
            else if (beatCount == 3)
                SafeMemCopy(drumBuf, snare.buf, snare.length, c, drumBufLength);
            else if (beatCount == 4)
                beatCount = 0;
            beatCount++;
        }
        break;
    case 5:
        std::cout << "Playing drum pattern: " << pickRandDrumPattern << " - Just a kick at the start of every bar \n";

        for (int c = 0; c < drumBufLength; c += songSettings.noteLenBytes)
        {
            if (beatCount == 4)
                beatCount = 0;
            
            if (beatCount == 1)
                SafeMemCopy(drumBuf, kick.buf, kick.length, c, drumBufLength);
            
            beatCount++;
        }
        break;
    case 6: // hi hat rolls - WTF is even going on here?
    {
        std::cout << "Playing drum pattern: " << pickRandDrumPattern << " - Hi hat rolls!!! \n";

        bool hatRoll = false;

        for (int c = 0; c < drumBufLength; c += songSettings.qtrNoteLenBytes)
        {
            if (hatRoll && (beatCount == 5 ||
                beatCount == 6 ||
                beatCount == 7 ||
                beatCount == 8 ||
                beatCount == 13))
                SafeMemCopy(drumBuf, hihat.buf, hihat.length, c, drumBufLength);

            if (beatCount == 1)
                SafeMemCopy(drumBuf, kickHat.buf, kickHat.length, c, drumBufLength);

            if (beatCount == 9)
                SafeMemCopy(drumBuf, snareHat.buf, snareHat.length, c, drumBufLength);

            if (!hatRoll && (beatCount == 5 ||
                beatCount == 13))
                SafeMemCopy(drumBuf, hihat.buf, hihat.length, c, drumBufLength);

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
    case 7:
        std::cout << "Playing drum pattern: " << pickRandDrumPattern << " - Kick Snare (slow) \n";

        for (int c = 0; c < drumBufLength; c += songSettings.noteLenBytes)
        {
            if (beatCount == 4)
            {
                barCount++;
                beatCount = 0;
            }
            
            if (( barCount == 1 || barCount == 3) && beatCount == 1)
                SafeMemCopy(drumBuf, kick.buf, kick.length, c, drumBufLength);
            
            if (( barCount == 2 || barCount == 4) && beatCount == 1)
                SafeMemCopy(drumBuf, snare.buf, snare.length, c, drumBufLength);
            
            beatCount++;
        }
        break;
    case 8: // Random 4/4
    {
        std::cout << "Playing drum pattern: " << pickRandDrumPattern << " - Random 4/4 \n";

        int drumChoice = mtRNG() % 4;
        AudioData randDrum1;
        if (drumChoice == 0)
            randDrum1 = kick;
        else if (drumChoice == 1)
            randDrum1 = snare;
        else if (drumChoice == 2)
            randDrum1 = hihat;
        
        drumChoice = mtRNG() % 4;
        AudioData randDrum2;
        if (drumChoice == 1)
            randDrum2 = kick;
        else if (drumChoice == 2)
            randDrum2 = snare;
        else if (drumChoice == 2)
            randDrum2 = hihat;
        
        drumChoice = mtRNG() % 4;
        AudioData randDrum3;
        if (drumChoice == 0)
            randDrum3 = kick;
        else if (drumChoice == 1)
            randDrum3 = snare;
        else if (drumChoice == 2)
            randDrum3 = hihat;
        
        drumChoice = mtRNG() % 5;
        AudioData randDrum4;
        if (drumChoice == 0)
            randDrum4 = kick;
        else if (drumChoice == 1)
            randDrum4 = snare;
        else if (drumChoice == 2)
            randDrum4 = hihat;
        
        for (int c = 0; c < drumBufLength; c += songSettings.qtrNoteLenBytes)
        {
            
            if (beatCount == 1)
                SafeMemCopy(drumBuf, randDrum1.buf, randDrum1.length, c, drumBufLength);
            
            if (beatCount == 5)
                SafeMemCopy(drumBuf, randDrum2.buf, randDrum2.length, c, drumBufLength);
            
            if (beatCount == 9)
                SafeMemCopy(drumBuf, randDrum3.buf, randDrum3.length, c, drumBufLength);
            
            if (beatCount == 13)
                SafeMemCopy(drumBuf, randDrum4.buf, randDrum4.length, c, drumBufLength);


            if (beatCount == 16)
            {
                beatCount = 0;
                barCount++;
            }
            beatCount++;
        }
    }
    break;
    case 9: 
        std::cout << "Playing drum pattern: " << pickRandDrumPattern << " - Kick Snare (slow) w/ dbl kick on 3rd bar \n";

        for (int c = 0; c < drumBufLength; c += songSettings.noteLenBytes)
        {
            if (beatCount == 4)
            {
                barCount++;
                beatCount = 0;
            }

            if ((barCount == 1 || barCount == 3) && beatCount == 1)
                SafeMemCopy(drumBuf, kick.buf, kick.length, c, drumBufLength);

            if ((barCount == 3) && beatCount == 2)
                SafeMemCopy(drumBuf, kick.buf, kick.length, c, drumBufLength);

            if ((barCount == 2 || barCount == 4) && beatCount == 1)
                SafeMemCopy(drumBuf, snare.buf, snare.length, c, drumBufLength);

            beatCount++;
        }
        break;
    case 10: 
        std::cout << "Playing drum pattern: " << pickRandDrumPattern << " - Classic kick + hat, hat, snare + hat, hat \n";

        for (int c = 0; c < drumBufLength; c += songSettings.noteLenBytes)
        {
            if (beatCount == 1)
                SafeMemCopy(drumBuf, kickHat.buf, kickHat.length, c, drumBufLength);
            else if (beatCount == 2)
                SafeMemCopy(drumBuf, hihat.buf, hihat.length, c, drumBufLength);
            else if (beatCount == 3)
                SafeMemCopy(drumBuf, snareHat.buf, snareHat.length, c, drumBufLength);
            else if (beatCount == 4)
            {
                SafeMemCopy(drumBuf, hihat.buf, hihat.length, c, drumBufLength);
                beatCount = 0;
                barCount++;
            }
            beatCount++;
        }
        break;
    case 11:
    {
        std::cout << "Playing drum pattern: " << pickRandDrumPattern << " - Kick & snare with hi-hat every beat \n";

        for (int c = 0; c < drumBufLength; c += (songSettings.halfNoteLenBytes))
        {
            if (beatCount == 1)
                SafeMemCopy(drumBuf, kickHat.buf, kickHat.length, c, drumBufLength);
            else if (beatCount == 2)
                SafeMemCopy(drumBuf, kick.buf, kick.length, c, drumBufLength);
            else if (beatCount == 5)
                SafeMemCopy(drumBuf, snareHat.buf, snareHat.length, c, drumBufLength);
            else if (beatCount == 3 || beatCount == 7)
                SafeMemCopy(drumBuf, hihat.buf, hihat.length, c, drumBufLength);

            beatCount++;
            if (beatCount == 8)
            {
                beatCount = 0;
                barCount++;
            }

        }
        break;
    }
    case 12:
    {
        std::cout << "Playing drum pattern: " << pickRandDrumPattern << " - Random 8 drum notes \n";

        int const beatsPerBar = 8;
        int beats[beatsPerBar];

        for (int c = 0; c < beatsPerBar; c++)
        {
            beats[c] = mtRNG() % 5;
        }

        AudioData randDrum;

        for (int c = 0; c < drumBufLength; c += songSettings.halfNoteLenBytes)
        {
            beatCount++;

            int drumChoice = beats[beatCount];

            if (drumChoice == 0)
                randDrum = kick;
            else if (drumChoice == 1)
                randDrum = snare;
            else if (drumChoice == 2)
                randDrum = hihat;

            if (drumChoice < 3)
                SafeMemCopy(drumBuf, randDrum.buf, randDrum.length, c, drumBufLength);

            if (beatCount == 8)
            {
                beatCount = 0;
                barCount++;
            }
        }
    }
    default:
        break;
    }

#ifdef DEBUG_DRUMS
    std::cout << "Final barCount: " << barCount << "\n";
	std::cout << "Final beatCount: " << beatCount << "\n";
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
    Noise(kickLength, true, waveBuffer, qtrMag / 2);

    // SineWave
#ifdef _WIN32 || _WIN64
    Sine(60, kickLength, fullMag, sineBuffer);
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


void TestDrums()
{
    SetupAudio();
    AudioData silenceSec = Silence(1000);
    AudioData jiffy = Silence(100);

    AudioData kick = GiveKick();
#ifdef DEBUG_DRUMS
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
#ifdef DEBUG_DRUMS
    DumpBuffer(hihat.buf, hihat.length, "hihat.txt");
#endif

    AudioPlayer(silenceSec);

    AudioData snare = GiveSnare();
    AudioPlayer(snare);
    AudioPlayer(jiffy);
    AudioPlayer(snare);

    if (songSettings.inited)
    {
        AudioData snareHat = songSettings.snareHatSound;
        AudioPlayer(snareHat);
        AudioPlayer(jiffy);
        AudioPlayer(snareHat);

        AudioData kickHat = songSettings.kickHatSound;
        AudioPlayer(kickHat);
        AudioPlayer(jiffy);
        AudioPlayer(kickHat);
    }

#ifdef DEBUG_DRUMS
    DumpBuffer(snare.buf, snare.length, "snare.txt");
#endif
}