//
//  Drums.cpp
//
//  Created by Morgan on 28/2/20.
//  Copyright © 2020 Morgan. All rights reserved.
//

#include "MusicGen.h"


void GenLeadTrack(Uint8* leadBuf, int leadBufLength)
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

    // Should we switch lead instruments?
    if (rand() % 15 == 6)
    {
        int instrument = rand() % 3;
        std::cout << "   > RNJesus wants to switch lead instruments to ";

        if (instrument == 0) // saw
        {
            std::cout << "sawtooth\n";
            songSettings.leadSine = false;
            songSettings.leadSquare = false;
            songSettings.leadSawtooth = true;

        }
        else if (instrument == 1) // sine
        {
            std::cout << "sine\n";
            songSettings.leadSine = true;
            songSettings.leadSquare = false;
            songSettings.leadSawtooth = false;
        }
        else // square
        {
            std::cout << "square\n";
            songSettings.leadSine = false;
            songSettings.leadSquare = true;
            songSettings.leadSawtooth = false;
        }
    }

    // Select lead pattern
    int pickRandLeadPattern;
    if (rand() % 4 == 0)
    {
        pickRandLeadPattern = songSettings.prevPatternLead;
        std::cout << "   > RNJesus wants the lead to play the same pattern as last time... \n";
    }
    else
    {
        pickRandLeadPattern = rand() % 20;

        // discourage these
        if (pickRandLeadPattern == 2 ||
            pickRandLeadPattern == 7 ||
            pickRandLeadPattern == 10 ||
            pickRandLeadPattern == 11 ||
            pickRandLeadPattern == 12 ||
            pickRandLeadPattern == 15)
            pickRandLeadPattern = rand() % 20;

        // really discourage these
        if (pickRandLeadPattern == 2 ||
            pickRandLeadPattern == 12)
            pickRandLeadPattern = rand() % 20;

        if (songSettings.BPM > 121 && pickRandLeadPattern == 8)
            pickRandLeadPattern = 0;
    }

    //DEBUG
     //pickRandLeadPattern = 18;
    //DEBUG

    songSettings.prevPatternLead = pickRandLeadPattern;

    std::cout << "Playing lead pattern: " << pickRandLeadPattern << "\n";

    // Here we go!
    int beatCount = 1;
    int barCount = 1;

    switch (pickRandLeadPattern) {
    case 0: // random through out, 8 halfnotes
    {
        for (int c = 0; c < leadBufLength; c += (songSettings.halfNoteLenBytes)) // 8 notes to bar
        {
            int chooseNote = rand() % 8;
            float noteFreq = key.freqs[chooseNote];
            SafeLead(noteFreq, songSettings.halfNoteLenMS, qtrMag / 2, leadBuf, c);

            if (beatCount == 8)
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
        for (int c = 0; c < leadBufLength; c += (songSettings.halfNoteLenBytes))
        {
            int chooseNote = rand() % 8;
            float noteFreq = key.freqs[chooseNote];

            if (beatCount % 2 == 1)
            {
                SafeLead(noteFreq, songSettings.halfNoteLenMS, qtrMag / 2, leadBuf, c);
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
    case 2: // generate & play note, skip a beat, play old note.  Repeat.  meh.
    {
        int chooseNote;
        float noteFreq = 0;
        int noteCount = 1;
        for (int c = 0; c < leadBufLength; c += (songSettings.halfNoteLenBytes))
        {

            if (noteCount % 3 == 1) // Generate a new note
            {
                chooseNote = rand() % 8;
                noteFreq = key.freqs[chooseNote];
                SafeLead(noteFreq, songSettings.halfNoteLenMS, qtrMag / 2, leadBuf, c);
            }
            else if (noteCount % 3 == 0) // Play the old note
            {
                SafeLead(noteFreq, songSettings.halfNoteLenMS, qtrMag / 2, leadBuf, c);
            } // else / 'skip a beat' is implied.

            if (beatCount == songSettings.barLenMS / songSettings.halfNoteLenMS)
            {
                beatCount = 0;
                barCount++;
            }
            beatCount++;
            noteCount++;
        }
        break;
    }
    case 3: // same as 0 but there is a chance of silence
    {
        for (int c = 0; c < leadBufLength; c += (songSettings.halfNoteLenBytes)) // 8 notes to bar
        {
            int chooseNote = rand() % 10; // chance of silence
            if (chooseNote < 8)
            {
                float noteFreq = key.freqs[chooseNote];
                SafeLead(noteFreq, songSettings.halfNoteLenMS, qtrMag / 2, leadBuf, c);
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
    case 4: // random through out, 4 notes per bar
    {
        for (int c = 0; c < leadBufLength; c += (songSettings.noteLenBytes)) // 4 beats
        {
            int chooseNote = rand() % 8;
            float noteFreq = key.freqs[chooseNote];
            SafeLead(noteFreq, songSettings.noteLenMS, qtrMag / 2, leadBuf, c);

            if (beatCount == 4)
            {
                beatCount = 0;
                barCount++;
            }
            beatCount++;
        }
        break;
    }
    case 5: // random through out, 4 notes per bar, fadeout
    {
        for (int c = 0; c < leadBufLength; c += (songSettings.noteLenBytes)) // 8 beats
        {
            int chooseNote = rand() % 8;
            float noteFreq = key.freqs[chooseNote];
            SafeLead(noteFreq, songSettings.noteLenMS, qtrMag / 2, leadBuf, c);
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
    case 6: // Silence
    {
        for (int c = 0; c < leadBufLength; c += (songSettings.halfNoteLenBytes)) // 8 notes to bar
        {
            if (beatCount == 8)
            {
                beatCount = 0;
                barCount++;
            }
            beatCount++;
        }
        break;
    }
    case 7: // 16th notes arps!
    {
        int noteCount = 0;
        int arpCount = 0;
        float firstInterval = 440.0f;
        float thirdInterval = 440.0f;
        float fifthInterval = 440.0f;
        float seventhInterval = 440.0f;
        bool randMode = false;
        int climbSpeed = songSettings.qtrNoteLenBytes;
        for (int c = 0; c < leadBufLength; c += climbSpeed) // 16 notes to bar
        {
            if (arpCount % 7 == 0 && !randMode)
            {
                float noteFreq = key.freqs[0];
                SafeLead(noteFreq, climbSpeed / audioSettings.bytesPerMS, qtrMag / 2, leadBuf, c);
            }
            else if (noteCount % 6 == 0 && arpCount % 7 == 1 && !randMode)
            {
                Scale tempKey = key;
                firstInterval = tempKey.freqs[0];
                thirdInterval = tempKey.freqs[2];
                fifthInterval = tempKey.freqs[4];
                seventhInterval = tempKey.freqs[6];
            }
            else if (noteCount % 6 == 0 && (arpCount % 7 == 2) && !randMode)
            {
                Scale tempKey(OppositeKeyMode(songSettings.key), key.freqs[5]);
                firstInterval = tempKey.freqs[0];
                thirdInterval = tempKey.freqs[2];
                fifthInterval = tempKey.freqs[4];
                seventhInterval = tempKey.freqs[6];

            }
            else if (noteCount % 6 == 0 && arpCount % 7 == 3 && !randMode)
            {
                Scale tempKey(songSettings.key, key.freqs[3]);
                firstInterval = tempKey.freqs[0];
                thirdInterval = tempKey.freqs[2];
                fifthInterval = tempKey.freqs[4];
                seventhInterval = tempKey.freqs[6];
            }
            else if (noteCount % 6 == 0 && arpCount % 7 == 4 && !randMode)
            {

                Scale tempKey(songSettings.key, key.freqs[4]);
                firstInterval = tempKey.freqs[0];
                thirdInterval = tempKey.freqs[2];
                fifthInterval = tempKey.freqs[4];
                seventhInterval = tempKey.freqs[6];
            }
            else if (noteCount % 6 == 0 && arpCount % 7 == 5 && !randMode)
            {
                Scale tempKey(songSettings.key, key.freqs[0]);
                firstInterval = tempKey.freqs[0];
                thirdInterval = tempKey.freqs[2];
                fifthInterval = tempKey.freqs[4];
                seventhInterval = tempKey.freqs[6];
            }
            else if (noteCount % 6 == 0 && arpCount % 7 == 6 && !randMode)
            {
                randMode = true;
                climbSpeed = songSettings.noteLenBytes;
            }


            if (randMode)
            {
                int chooseNote = rand() % 8;
                float noteFreq = key.freqs[chooseNote];
                SafeLead(noteFreq, climbSpeed / audioSettings.bytesPerMS, qtrMag / 2, leadBuf, c);
            }
            else {
                if (noteCount % 6 == 0)
                    SafeLead(firstInterval, songSettings.qtrNoteLenMS, qtrMag / 2, leadBuf, c);
                else if (noteCount % 6 == 1)
                    SafeLead(thirdInterval, songSettings.qtrNoteLenMS, qtrMag / 2, leadBuf, c);
                else if (noteCount % 6 == 2)
                    SafeLead(fifthInterval, songSettings.qtrNoteLenMS, qtrMag / 2, leadBuf, c);
                else if (noteCount % 6 == 3)
                    SafeLead(seventhInterval, songSettings.qtrNoteLenMS, qtrMag / 2, leadBuf, c);
                else if (noteCount % 6 == 4)
                    SafeLead(fifthInterval, songSettings.qtrNoteLenMS, qtrMag / 2, leadBuf, c);
                else if (noteCount % 6 == 5)
                {
                    SafeLead(thirdInterval, songSettings.qtrNoteLenMS, qtrMag / 2, leadBuf, c);
                    //std::cout << "Arp Count: " << arpCount << "\n";
                    arpCount++;
                }
            }

            if (beatCount == 16)
            {
                beatCount = 0;
                //std::cout << "bar Count: " << barCount << "\n";
                barCount++;
            }
            beatCount++;
            noteCount++;
        }
        break;
    }
    case 8: // 16th notes! Only play when bpm is 120 or under
    {
        for (int c = 0; c + songSettings.qtrNoteLenBytes < leadBufLength; c += (songSettings.qtrNoteLenBytes)) // 16 notes to bar
        {
            int chooseNote = rand() % 9;

            if (chooseNote < 8)
            {
                float noteFreq = key.freqs[chooseNote];
                SafeLead(noteFreq, songSettings.qtrNoteLenMS, qtrMag / 2, leadBuf, c);
            }

            if (beatCount == 16)
            {
                beatCount = 0;
                barCount++;
            }
            beatCount++;
        }
        break;
    }
    case 9: // play a qtr note, wait qtr note, play another qtr half note, etc.
    {
        for (int c = 0; c < leadBufLength; c += (songSettings.qtrNoteLenBytes))
        {

            if (beatCount % 2 == 1)
            {
                int chooseNote = rand() % 8;
                float noteFreq = key.freqs[chooseNote];
                SafeLead(noteFreq, songSettings.qtrNoteLenMS, qtrMag / 2, leadBuf, c);
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
    case 10: // random through out, 8 notes per bar, fadeout
    {
        for (int c = 0; c < leadBufLength; c += leadBufLength) // 1 beat per bar
        {
            int chooseNote = rand() % 8;
            float noteFreq = key.freqs[chooseNote];
            SafeLead(noteFreq, leadBufLength / audioSettings.bytesPerMS, qtrMag / 2, leadBuf, c);
            if (songSettings.prevPatternLead == 6)
                SafeFadeIn(leadBuf, leadBufLength, c);
            else
                SafeFadeOut(leadBuf, leadBufLength, c);


            if (beatCount == 1)
            {
                beatCount = 0;
                barCount++;
            }
            beatCount++;
        }
        break;
    }
    case 11: // 8th notes arps!
    {
        int noteCount = 0;
        int arpCount = 0;
        float firstInterval = 440.0f;
        float thirdInterval = 440.0f;
        float fifthInterval = 440.0f;
        float seventhInterval = 440.0f;
        bool randMode = false;
        int climbSpeed = songSettings.halfNoteLenBytes;
        for (int c = 0; c < leadBufLength; c += climbSpeed) // 8 notes to bar
        {
            if (arpCount % 7 == 0 && !randMode)
            {
                float noteFreq = key.freqs[0];
                SafeLead(noteFreq, climbSpeed / audioSettings.bytesPerMS, qtrMag / 2, leadBuf, c);
            }
            else if (noteCount % 6 == 0 && arpCount % 7 == 1 && !randMode)
            {
                Scale tempKey = key;
                firstInterval = tempKey.freqs[0];
                thirdInterval = tempKey.freqs[2];
                fifthInterval = tempKey.freqs[4];
                seventhInterval = tempKey.freqs[6];
            }
            else if (noteCount % 6 == 0 && (arpCount % 7 == 2) && !randMode)
            {
                Scale tempKey(OppositeKeyMode(songSettings.key), key.freqs[5]);
                firstInterval = tempKey.freqs[0];
                thirdInterval = tempKey.freqs[2];
                fifthInterval = tempKey.freqs[4];
                seventhInterval = tempKey.freqs[6];

            }
            else if (noteCount % 6 == 0 && arpCount % 7 == 3 && !randMode)
            {
                Scale tempKey(songSettings.key, key.freqs[3]);
                firstInterval = tempKey.freqs[0];
                thirdInterval = tempKey.freqs[2];
                fifthInterval = tempKey.freqs[4];
                seventhInterval = tempKey.freqs[6];
            }
            else if (noteCount % 6 == 0 && arpCount % 7 == 4 && !randMode)
            {

                Scale tempKey(songSettings.key, key.freqs[4]);
                firstInterval = tempKey.freqs[0];
                thirdInterval = tempKey.freqs[2];
                fifthInterval = tempKey.freqs[4];
                seventhInterval = tempKey.freqs[6];
            }
            else if (noteCount % 6 == 0 && arpCount % 7 == 5 && !randMode)
            {
                Scale tempKey(songSettings.key, key.freqs[0]);
                firstInterval = tempKey.freqs[0];
                thirdInterval = tempKey.freqs[2];
                fifthInterval = tempKey.freqs[4];
                seventhInterval = tempKey.freqs[6];
            }
            else if (noteCount % 6 == 0 && arpCount % 7 == 6 && !randMode)
            {
                randMode = true;
                climbSpeed = songSettings.noteLenBytes;
            }


            if (randMode)
            {
                int chooseNote = rand() % 8;
                float noteFreq = key.freqs[chooseNote];
                SafeLead(noteFreq, climbSpeed / audioSettings.bytesPerMS, qtrMag / 2, leadBuf, c);
            }
            else {
                if (noteCount % 6 == 0)
                    SafeLead(firstInterval, songSettings.halfNoteLenMS, qtrMag / 2, leadBuf, c);
                else if (noteCount % 6 == 1)
                    SafeLead(thirdInterval, songSettings.halfNoteLenMS, qtrMag / 2, leadBuf, c);
                else if (noteCount % 6 == 2)
                    SafeLead(fifthInterval, songSettings.halfNoteLenMS, qtrMag / 2, leadBuf, c);
                else if (noteCount % 6 == 3)
                    SafeLead(seventhInterval, songSettings.halfNoteLenMS, qtrMag / 2, leadBuf, c);
                else if (noteCount % 6 == 4)
                    SafeLead(fifthInterval, songSettings.halfNoteLenMS, qtrMag / 2, leadBuf, c);
                else if (noteCount % 6 == 5)
                {
                    SafeLead(thirdInterval, songSettings.halfNoteLenMS, qtrMag / 2, leadBuf, c);
                    //std::cout << "Arp Count: " << arpCount << "\n";
                    arpCount++;
                }
            }

            if (beatCount == 16)
            {
                beatCount = 0;
                //std::cout << "bar Count: " << barCount << "\n";
                barCount++;
            }
            beatCount++;
            noteCount++;
        }
        break;
    }
    case 12: // 8th notes arps!
    {
        int arpCount = 1;


        for (int c = 0; c < leadBufLength; c += songSettings.dblNoteLenBytes)
        {

            /*if (arpCount == 1)
            {
                Scale tempKey = key;
                GenArp(tempKey.notes["1st"], songSettings.dblNoteLenMS, 8, halfMag, leadBuf, c, false);

            }
            else if (arpCount == 2)
            {
                Scale tempKey(songSettings.key, key.freqs[4]);
                GenArp(tempKey.notes["1st"], songSettings.dblNoteLenMS, 16, halfMag, leadBuf, c, false);
            }
            else if (arpCount == 3)
            {
                Scale tempKey(SwitchKeyMode(songSettings.key), key.freqs[5]);
                GenArp(tempKey.notes["1st"], songSettings.dblNoteLenMS, 32, halfMag, leadBuf, c, false);
            }
            else if (arpCount == 4)
            {
                Scale tempKey(songSettings.key, key.freqs[3]);
                GenArp(tempKey.notes["1st"], songSettings.dblNoteLenMS, 16, halfMag, leadBuf, c, false);
            }
            int speed = rand() % 3;
            if (speed == 0)
                speed = 8;
            else if (speed == 1)
                speed = 16;
            else
                speed = 32;
            */

            int speed = 16;

            Scale tempKey(songSettings.key, key.freqs[rand() % 8]);
            GenArp(tempKey.notes["1st"], songSettings.dblNoteLenMS, speed, halfMag, leadBuf, c, false);



            arpCount++;

            if (beatCount == 4)
            {
                beatCount = 0;
                //std::cout << "bar Count: " << barCount << "\n";
                barCount++;
            }
            beatCount++;
        }
        break;
    }
    case 13: // 8th notes arps!
    {
        float Note1 = key.freqs[rand() % 8];
        float Note2 = key.freqs[rand() % 8];
        float Note3 = key.freqs[rand() % 8];
        float Note4 = key.freqs[rand() % 8];


        for (int c = 0; c < leadBufLength; c += songSettings.halfNoteLenBytes)
        {

            if (barCount == 1)
                SafeLead(Note1, songSettings.halfNoteLenMS, qtrMag / 2, leadBuf, c);
            if (barCount == 2)
                SafeLead(Note2, songSettings.halfNoteLenMS, qtrMag / 2, leadBuf, c);
            if (barCount == 3)
                SafeLead(Note3, songSettings.halfNoteLenMS, qtrMag / 2, leadBuf, c);
            if (barCount == 4)
                SafeLead(Note4, songSettings.halfNoteLenMS, qtrMag / 2, leadBuf, c);


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
    case 14: // play a qtr note, wait qtr note, play another qtr half note, etc.
    {
        for (int c = 0; c < leadBufLength; c += (songSettings.qtrNoteLenBytes))
        {

            if (beatCount % 2 == 1)
            {
                int chooseNote = rand() % 9;
                if (chooseNote < 8)
                {
                    float noteFreq = key.freqs[chooseNote];
                    SafeLead(noteFreq, songSettings.qtrNoteLenMS, qtrMag / 2, leadBuf, c);
                }
            }

            if (beatCount == 16)
            {
                beatCount = 0;
                barCount++;
            }
            beatCount++;
        }
        break;
    case 15: // play a qtr note, wait qtr note, play another qtr half note, etc.
    {
        for (int c = 0; c < leadBufLength; c += (songSettings.barLenBytes))
        {

            if (barCount == 4 || barCount == 3)
            {
                SlideSquare(key.freqs[7], key.freqs[0], songSettings.barLenMS, qtrMag / 2, leadBuf, c);
            }

            if (beatCount == 1)
            {
                beatCount = 0;
                barCount++;
            }
            beatCount++;
        }
        break;
    }
    case 16: // random through out, 4 notes per bar
    {
        float lastNoteFreq = 0.0f;
        float noteFreq = 0.0f;

        for (int c = 0; c < leadBufLength; c += (songSettings.noteLenBytes)) // 4 beats
        {

            if (barCount == 1 && beatCount == 1)
            {
                int chooseNote = rand() % 8;
                noteFreq = key.freqs[chooseNote];
                SafeSquare(noteFreq, songSettings.noteLenMS, qtrMag / 2, leadBuf, c);
                lastNoteFreq = noteFreq;
            }
            else if (beatCount % 2 == 1 && !(beatCount == 1))
            {
                SafeSquare(noteFreq, songSettings.noteLenMS, qtrMag / 2, leadBuf, c);
                lastNoteFreq = noteFreq;

            }
            else
            {
                int chooseNote = rand() % 8;
                noteFreq = key.freqs[chooseNote];
                SlideSquare(lastNoteFreq, noteFreq, songSettings.noteLenMS, qtrMag / 2, leadBuf, c);
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
    case 17: // random through out, 4 notes per bar
    {
        int chooseNote = rand() % 8;
        float freq1 = key.freqs[chooseNote];
        chooseNote = rand() % 8;
        float freq2 = key.freqs[chooseNote];
        chooseNote = rand() % 8;
        float freq3 = key.freqs[chooseNote];

        for (int c = 0; c < leadBufLength; c += (songSettings.noteLenBytes)) // 4 beats
        {
            chooseNote = rand() % 4;
            if (chooseNote == 0)
            {
                SafeLead(freq1, songSettings.noteLenMS, qtrMag / 2, leadBuf, c);
                SafeFadeIn(leadBuf, songSettings.noteLenBytes, c);
            }

            else if (chooseNote == 1)
            {
                SafeLead(freq2, songSettings.noteLenMS, qtrMag / 2, leadBuf, c);
                SafeFadeIn(leadBuf, songSettings.noteLenBytes, c);
            }

            else if (chooseNote == 2)
            {
                SafeLead(freq3, songSettings.noteLenMS, qtrMag / 2, leadBuf, c);
                SafeFadeIn(leadBuf, songSettings.noteLenBytes, c);
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
    case 18: // random through out, 4 notes per bar
    {
        int chooseNote = rand() % 8;
        float freq1 = key.freqs[chooseNote];
        chooseNote = rand() % 8;
        float freq2 = key.freqs[chooseNote];
        chooseNote = rand() % 8;
        float freq3 = key.freqs[chooseNote];

        for (int c = 0; c < leadBufLength; c += (songSettings.noteLenBytes)) // 4 beats
        {
            chooseNote = rand() % 4;
            if (chooseNote == 0)
                SafeLead(freq1, songSettings.noteLenMS, qtrMag / 2, leadBuf, c);
            else if (chooseNote == 1)
                SafeLead(freq2, songSettings.noteLenMS, qtrMag / 2, leadBuf, c);
            else if (chooseNote == 2)
                SafeLead(freq3, songSettings.noteLenMS, qtrMag / 2, leadBuf, c);

            if (beatCount == 4)
            {
                beatCount = 0;
                barCount++;
            }
            beatCount++;
        }
        break;
    }
    case 19: // random through out, 4 notes per bar
    {
        int chooseNote = rand() % 8;
        float freq1 = key.freqs[chooseNote];
        chooseNote = rand() % 8;
        float freq2 = key.freqs[chooseNote];

        for (int c = 0; c < leadBufLength; c += (songSettings.halfNoteLenBytes)) // 4 beats
        {

            if (rand() % 2 == 1)
                SafeLead(freq1, songSettings.halfNoteLenBytes, qtrMag / 2, leadBuf, c);
            else
                SafeLead(freq2, songSettings.halfNoteLenBytes, qtrMag / 2, leadBuf, c);


            if (beatCount == 4)
            {
                beatCount = 0;
                barCount++;
            }
            beatCount++;
        }
        break;
    }

    }
    default:
        break;
    }

#ifdef DEBUG_AUDIO
    DumpBuffer(leadBuf, leadBufLength, "LeadBuffer.txt");
#endif
}

