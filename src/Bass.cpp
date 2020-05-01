//
//  Drums.cpp
//
//  Created by Morgan on 28/2/20.
//  Copyright © 2020 Morgan. All rights reserved.
//

#include "MusicGen.h"


void GenBassTrack(Uint8* bassBuf, int bassBufLength)
{
    // Get base note for bass
    /* std::string bassScaleNote;
    bassScaleNote.append(songSettings.scaleNote);
    bassScaleNote.append("1"); // octave for bass
    
#ifdef DEBUG_AUDIO
    std::cout << "Bass base scale note: " << bassScaleNote << "\n";
#endif
    */
    
    // Construct key scale
    Scale key(songSettings.scaleType, songSettings.bassBaseScaleFreq);
    
    // Select bass pattern
    int pickRandBassPattern;
    if (mtRNG() % 3 == 0)
    {
        std::cout << "   > RNJesus wants the bass to play the same pattern as last time... \n";
        pickRandBassPattern = songSettings.prevPatternBass;
    }
    else
        pickRandBassPattern = mtRNG() % 11;

    // discourage these
    if (pickRandBassPattern == 1 || pickRandBassPattern == 4) {
        //std::cout << "Selected discouraged bass pattern: " << pickRandBassPattern << ". Rerolling...\n";
        pickRandBassPattern = mtRNG() % 11;
    }

    songSettings.prevPatternBass = pickRandBassPattern;

    //DEBUG
    //pickRandBassPattern = 5;
    //DEBUG


    std::cout << "Playing bass pattern: " << pickRandBassPattern << "\n";

    // Here we go!
    int beatCount = 1;
    int barCount = 1;
    
    switch (pickRandBassPattern) {
    case 0: // 1 note per bar
    {
        for (int c = 0; c < bassBufLength; c += (songSettings.barLenMS * audioSettings.samplesPerMS * 2)) // 1 note per bar
        {
            int chooseNote = mtRNG() % 8;
            float noteFreq = key.freqs[chooseNote];
            SafeSawtooth(noteFreq, songSettings.barLenMS, qtrMag, bassBuf, c);

            if (beatCount == 1)
            {
                beatCount = 0;
                barCount++;
            }
            beatCount++;
        }
        break;
    }
    case 1: // play a half note, wait, play another half note.  Could actually be done more efficiently
    {
        for (int c = 0; c < bassBufLength; c += (songSettings.halfNoteLenBytes))
        {
            int chooseNote = mtRNG() % 8;
            float noteFreq = key.freqs[chooseNote];
            
            if (beatCount % 2 == 1)
            {
                SafeSawtooth(noteFreq, songSettings.halfNoteLenMS, qtrMag, bassBuf, c);
            }

            if (beatCount == 8)
            {
                beatCount = 0;
                barCount++;
            }
            beatCount++;
        }
        break;
    }
    case 2: // Change note every 2 bars, 2 notes per bar with silence in between
    {
        int chooseNote;
        float noteFreq = 0;
        for (int c = 0; c < bassBufLength; c += (songSettings.noteLenBytes))
        {
            if ((barCount == 1 || barCount == 3) && beatCount == 1)
            {
                chooseNote = mtRNG() % 8;
                noteFreq = key.freqs[chooseNote];
            }

            if (beatCount % 2 == 1)
            {
                SafeSawtooth(noteFreq, songSettings.noteLenMS, qtrMag, bassBuf, c);
            }

            if (beatCount == songSettings.barLenMS / songSettings.noteLenMS)
            {
                beatCount = 0;
                barCount++;
            }
            beatCount++;
        }
        break;
    }
    case 3: // random through out, 4 notes per bar
    {
        for (int c = 0; c < bassBufLength; c += (songSettings.noteLenBytes)) // 4 beats
        {
            int chooseNote = mtRNG() % 8;
            float noteFreq = key.freqs[chooseNote];
            SafeSawtooth(noteFreq, songSettings.noteLenMS, qtrMag, bassBuf, c);

            if (beatCount == 4)
            {
                beatCount = 0;
                barCount++;
            }
            beatCount++;
        }
        break;
    }
    case 4: // random through out, 4 notes per bar, fadeout
    {
        for (int c = 0; c < bassBufLength; c += (songSettings.dblNoteLenBytes)) // 8 beats
        {
            int chooseNote = mtRNG() % 8;
            float noteFreq = key.freqs[chooseNote];
            SafeSawtooth(noteFreq, songSettings.dblNoteLenMS, qtrMag, bassBuf, c);
            SafeFadeOut(bassBuf, songSettings.noteLenBytes, c + songSettings.noteLenBytes); // attenuate the second half of the note

            if (beatCount == 2)
            {
                beatCount = 0;
                barCount++;
            }
            beatCount++;
        }
        break;
    }
    case 5:
    {
        float Note1 = key.freqs[mtRNG() % 8];
        float Note2 = key.freqs[mtRNG() % 8];
        float Note3 = key.freqs[mtRNG() % 8];
        float Note4 = key.freqs[mtRNG() % 8];


        for (int c = 0; c < bassBufLength; c += songSettings.halfNoteLenBytes)
        {

            if (barCount == 1)
                SafeSawtooth(Note1, songSettings.halfNoteLenMS - 10, qtrMag, bassBuf, c);
            if (barCount == 2)
                SafeSawtooth(Note2, songSettings.halfNoteLenMS - 10, qtrMag, bassBuf, c);
            if (barCount == 3)
                SafeSawtooth(Note3, songSettings.halfNoteLenMS - 10, qtrMag, bassBuf, c);
            if (barCount == 4)
                SafeSawtooth(Note4, songSettings.halfNoteLenMS - 10, qtrMag, bassBuf, c);


            if (beatCount == 8)
            {
                beatCount = 0;
                //std::cout << "bar Count: " << barCount << "\n";
                barCount++;
            }
            beatCount++;
        }
        break;
    }
    case 6: // 1 note per bar.  This is a repeat of 0 as it's a good one except fade out last bar
    {
        for (int c = 0; c < bassBufLength; c += songSettings.barLenBytes) // 1 note per bar
        {
            int chooseNote = mtRNG() % 8;
            float noteFreq = key.freqs[chooseNote];
            SafeSawtooth(noteFreq, songSettings.barLenMS, qtrMag, bassBuf, c);

            if (barCount == 4)
                SafeFadeOut(bassBuf, songSettings.barLenBytes, c);

            if (beatCount == 1)
            {
                beatCount = 0;
                barCount++;
            }
            beatCount++;
        }
        break;
    }
    case 7: // 1 note per 2x bar.
    {
        for (int c = 0; c < bassBufLength; c += (songSettings.barLenMS * audioSettings.samplesPerMS * 4)) // 1 note per 2x bar
        {
            int chooseNote = mtRNG() % 8;
            float noteFreq = key.freqs[chooseNote];
            SafeSawtooth(noteFreq, songSettings.barLenMS * 2, qtrMag, bassBuf, c);

            if (beatCount / 2 == 1)
            {
                beatCount = 0;
                barCount++;
            }
            beatCount++;
        }
        break;
    }
    case 8: // bass change note every note but can also skip
    {
        for (int c = 0; c < bassBufLength; c += (songSettings.noteLenBytes))
        {

            //if (beatCount % 2 == 1)
            //{
                int chooseNote = mtRNG() % 9;
                if (chooseNote < 8)
                {
                    float noteFreq = key.freqs[chooseNote];
                    SafeSawtooth(noteFreq, songSettings.noteLenMS, qtrMag, bassBuf, c);
                }
            //}

            if (beatCount == 4)
            {
                beatCount = 0;
                barCount++;
            }
            beatCount++;
    }
        break;
    }
    case 9: // bass alternate between two
    {
        int chooseNote = mtRNG() % 8;
        float freq1 = key.freqs[chooseNote];
        chooseNote = mtRNG() % 8;
        float freq2 = key.freqs[chooseNote];

        // Discourage the same note from being used but don't ban it.
        if (freq2 == freq1)
        {
            chooseNote = mtRNG() % 8;
            float freq2 = key.freqs[chooseNote];
        }

        for (int c = 0; c < bassBufLength; c += (songSettings.noteLenBytes)) // 4 beats
        {

            if (beatCount % 2 == 1)
                SafeSawtooth(freq1, songSettings.noteLenBytes, qtrMag, bassBuf, c);
            else
                SafeSawtooth(freq2, songSettings.noteLenBytes, qtrMag, bassBuf, c);


            if (beatCount == 1 && !(barCount == 1)) {
                chooseNote = mtRNG() % 8;
                freq1 = key.freqs[chooseNote];
                chooseNote = mtRNG() % 8;
                freq2 = key.freqs[chooseNote];
            }

            if (beatCount == 4)
            {
                beatCount = 0;
                barCount++;
            }
            beatCount++;
        }
        break;
    }
    case 10: // almost repeat of 5 cause it's good
    {
        float Note1 = key.freqs[mtRNG() % 8];
        float Note2 = key.freqs[mtRNG() % 8];
        float Note3 = key.freqs[mtRNG() % 8];
        float Note4 = key.freqs[mtRNG() % 8];


        for (int c = 0; c < bassBufLength; c += songSettings.halfNoteLenBytes)
        {

            if (barCount == 1)
                SafeSawtooth(Note1, songSettings.halfNoteLenMS - 15, qtrMag, bassBuf, c);
            if (barCount == 2)
                SafeSawtooth(Note2, songSettings.halfNoteLenMS - 15, qtrMag, bassBuf, c);
            if (barCount == 3)
                SafeSawtooth(Note3, songSettings.halfNoteLenMS - 15, qtrMag, bassBuf, c);
            if (barCount == 4)
                SafeSawtooth(Note4, songSettings.halfNoteLenMS - 15, qtrMag, bassBuf, c);


            if (beatCount == 8)
            {
                beatCount = 0;
                //std::cout << "bar Count: " << barCount << "\n";
                barCount++;
            }
            beatCount++;
        }
        break;
    }
    default:
        break;
    }

#ifdef DEBUG_AUDIO
    DumpBuffer(bassBuf, bassBufLength, "BassBuffer.txt");
#endif
}





