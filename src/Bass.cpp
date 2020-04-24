//
//  Drums.cpp
//
//  Created by Morgan on 28/2/20.
//  Copyright © 2020 Morgan. All rights reserved.
//

#include "MusicGen.h"


void GenBassTrack(Uint8* bassBuf, int drumBufLength)
{
    // Get base note for bass
    std::string bassScaleNote;
    bassScaleNote.append(songSettings.keyNote);
    bassScaleNote.append("1"); // octave for bass
    
#ifdef DEBUG_AUDIO
    std::cout << "Bass base scale note: " << bassScaleNote << "\n";
#endif
    
    // Construct key scale
    Scale key(songSettings.key, Notes.getNoteFreq(bassScaleNote));
    
    // Select bass pattern
    int pickRandBassPattern;
    if (rand() % 3 == 0)
    {
        std::cout << "   > RNJesus wants the bass to play the same pattern as last time... \n";
        pickRandBassPattern = songSettings.prevPatternBass;
    }

    else
        pickRandBassPattern = rand() % 4;
    songSettings.prevPatternBass = pickRandBassPattern;


    std::cout << "Playing bass pattern: " << pickRandBassPattern << "\n";

    // Here we go!
    int beatCount = 1;
    int barCount = 1;
    
    switch (pickRandBassPattern) {
    case 0: // 1 note per bar
    {
        for (int c = 0; c < drumBufLength; c += (songSettings.barLenMS * audioSettings.samplesPerMS * 2)) // 1 note per bar
        {
            int chooseNote = rand() % 8;
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
        for (int c = 0; c < drumBufLength; c += (songSettings.halfNoteLenBytes))
        {
            int chooseNote = rand() % 8;
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
        for (int c = 0; c < drumBufLength; c += (songSettings.noteLenBytes))
        {
            if ((barCount == 1 || barCount == 3) && beatCount == 1)
            {
                chooseNote = rand() % 8;
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
        for (int c = 0; c < drumBufLength; c += (songSettings.noteLenBytes)) // 4 beats
        {
            int chooseNote = rand() % 8;
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
        for (int c = 0; c < drumBufLength; c += (songSettings.noteLenBytes)) // 8 beats
        {
            int chooseNote = rand() % 8;
            float noteFreq = key.freqs[chooseNote];
            SafeSawtooth(noteFreq, songSettings.noteLenMS, qtrMag, bassBuf, c);
            SafeFadeOut(bassBuf, songSettings.noteLenBytes, c);

            if (beatCount == 4)
            {
                beatCount = 0;
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
    DumpBuffer(bassBuf, drumBufLength, "BassBuffer.txt");
#endif
}





