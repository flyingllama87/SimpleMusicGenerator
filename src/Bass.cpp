//
//  Drums.cpp
//
//  Created by Morgan on 28/2/20.
//  Copyright © 2020 Morgan. All rights reserved.
//

#include "MusicGen.h"


void GenBassTrack(Uint8* bassBuf)
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
    int pickRandBassPattern = rand() % 7;
    std::cout << "Playing bass pattern: " << pickRandBassPattern << "\n";

    // Here we go!
    int beatCount = 1;
    int barCount = 1;
    
    switch (pickRandBassPattern) {
    case 0: // random through out, 8 halfnotes
    {
        for (int c = 0; c < internalAudioBuffer.length; c += (songSettings.halfNoteLenBytes)) // 8 notes to bar
        {
            int chooseNote = rand() % 8;
            float noteFreq = key.freqs[chooseNote];
            SafeSawtooth(noteFreq, songSettings.halfNoteLenMS, qtrMag, bassBuf, c);

            if (beatCount == 8)
            {
                beatCount = 0;
                barCount++;
            }
            beatCount++;
        }
        break;
    }
    case 1: // 1 note per bar
    {
        for (int c = 0; c < internalAudioBuffer.length; c += (songSettings.barLenMS * audioSettings.samplesPerMS * 2)) // 1 note per bar
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
    case 2: // play a half note, wait, play another half note.  Could actually be done more efficiently
    {
        for (int c = 0; c < internalAudioBuffer.length; c += (songSettings.halfNoteLenBytes))
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
    case 3: // Change note every 2 bars, 2 notes per bar with silence in between
    {
        int chooseNote;
        float noteFreq = 0;
        for (int c = 0; c < internalAudioBuffer.length; c += (songSettings.noteLenBytes))
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
    case 4: // Copy of case 0.  Was silence.  Maybe add in when we have lead in.
    {
        for (int c = 0; c < internalAudioBuffer.length; c += (songSettings.halfNoteLenBytes)) // 8 notes to bar
        {
            int chooseNote = rand() % 8;
            float noteFreq = key.freqs[chooseNote];
            SafeSawtooth(noteFreq, songSettings.halfNoteLenMS, qtrMag, bassBuf, c);

            if (beatCount == 8)
            {
                beatCount = 0;
                barCount++;
            }
            beatCount++;
        }
        break;
    }
    break;
    case 5: // random through out, 4 notes per bar
    {
        for (int c = 0; c < internalAudioBuffer.length; c += (songSettings.noteLenBytes)) // 4 beats
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
    case 6: // random through out, 4 notes per bar, fadeout
    {
        for (int c = 0; c < internalAudioBuffer.length; c += (songSettings.noteLenBytes)) // 8 beats
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
    DumpBuffer(bassBuf, internalAudioBuffer.length, "BassBuffer.txt");
#endif
}


void GenLeadTrack(Uint8* leadBuf)
{
    // Get base note for lead
    std::string leadScaleNote;
    leadScaleNote.append(songSettings.keyNote);
    leadScaleNote.append("4"); // octave for lead
    
#ifdef DEBUG_AUDIO
    std::cout << "Lead base scale note: " << leadScaleNote << "\n";
#endif
    
    // Construct key scale
    Scale key(songSettings.key, Notes.getNoteFreq(leadScaleNote));
    
    // Select lead pattern
    int pickRandLeadPattern = rand() % 7;
    std::cout << "Playing lead pattern: " << pickRandLeadPattern << "\n";

    // Here we go!
    int beatCount = 1;
    int barCount = 1;
    
    switch (pickRandLeadPattern) {
    case 0: // random through out, 8 halfnotes
    {
        for (int c = 0; c < internalAudioBuffer.length; c += (songSettings.halfNoteLenBytes)) // 8 notes to bar
        {
            int chooseNote = rand() % 8;
            float noteFreq = key.freqs[chooseNote];
            SafeSquare(noteFreq, songSettings.halfNoteLenMS, qtrMag, leadBuf, c);

            if (beatCount == 8)
            {
                beatCount = 0;
                barCount++;
            }
            beatCount++;
        }
        break;
    }
    case 1: // 1 note per bar
    {
        for (int c = 0; c < internalAudioBuffer.length; c += (songSettings.barLenMS * audioSettings.samplesPerMS * 2)) // 1 note per bar
        {
            int chooseNote = rand() % 8;
            float noteFreq = key.freqs[chooseNote];
            SafeSquare(noteFreq, songSettings.barLenMS, qtrMag, leadBuf, c);

            if (beatCount == 1)
            {
                beatCount = 0;
                barCount++;
            }
            beatCount++;
        }
        break;
    }
    case 2: // play a half note, wait, play another half note.  Could actually be done more efficiently
    {
        for (int c = 0; c < internalAudioBuffer.length; c += (songSettings.halfNoteLenBytes))
        {
            int chooseNote = rand() % 8;
            float noteFreq = key.freqs[chooseNote];
            
            if (beatCount % 2 == 1)
            {
                SafeSquare(noteFreq, songSettings.halfNoteLenMS, qtrMag, leadBuf, c);
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
    case 3: // Change note every 2 bars, 2 notes per bar with silence in between
    {
        int chooseNote;
        float noteFreq = 0;
        for (int c = 0; c < internalAudioBuffer.length; c += (songSettings.noteLenBytes))
        {
            if ((barCount == 1 || barCount == 3) && beatCount == 1)
            {
                chooseNote = rand() % 8;
                noteFreq = key.freqs[chooseNote];
            }

            if (beatCount % 2 == 1)
            {
                SafeSquare(noteFreq, songSettings.noteLenMS, qtrMag, leadBuf, c);
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
    case 4: // Copy of case 0.  Was silence.  Maybe add in when we have lead in.
    {
        for (int c = 0; c < internalAudioBuffer.length; c += (songSettings.halfNoteLenBytes)) // 8 notes to bar
        {
            int chooseNote = rand() % 8;
            float noteFreq = key.freqs[chooseNote];
            SafeSquare(noteFreq, songSettings.halfNoteLenMS, qtrMag, leadBuf, c);

            if (beatCount == 8)
            {
                beatCount = 0;
                barCount++;
            }
            beatCount++;
        }
        break;
    }
    break;
    case 5: // random through out, 4 notes per bar
    {
        for (int c = 0; c < internalAudioBuffer.length; c += (songSettings.noteLenBytes)) // 4 beats
        {
            int chooseNote = rand() % 8;
            float noteFreq = key.freqs[chooseNote];
            SafeSquare(noteFreq, songSettings.noteLenMS, qtrMag, leadBuf, c);

            if (beatCount == 4)
            {
                beatCount = 0;
                barCount++;
            }
            beatCount++;
        }
        break;
    }
    case 6: // random through out, 4 notes per bar, fadeout
    {
        for (int c = 0; c < internalAudioBuffer.length; c += (songSettings.noteLenBytes)) // 8 beats
        {
            int chooseNote = rand() % 8;
            float noteFreq = key.freqs[chooseNote];
            SafeSquare(noteFreq, songSettings.noteLenMS, qtrMag, leadBuf, c);
            SafeFadeOut(leadBuf, songSettings.noteLenBytes, c);

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
    DumpBuffer(leadBuf, internalAudioBuffer.length, "LeadBuffer.txt");
#endif
}
