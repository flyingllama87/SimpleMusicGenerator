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

    /*std::string leadScaleNote;
    leadScaleNote.append(songSettings.scaleNote);
    leadScaleNote.append("4"); // octave for lead

#ifdef DEBUG_AUDIO
    std::cout << "Lead base scale note: " << leadScaleNote << "\n";
#endif
    */

    // Construct key scale
    Scale key(songSettings.scaleType, songSettings.leadBaseScaleFreq);

    // Should we switch lead instruments?
    if (mtRNG() % 15 == 6 || (songSettings.leadSine == true && mtRNG() % 5 == 4))
    {
        int instrument = mtRNG() % 3;
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

    // switch lead between 3rd and 4th octave? 250f is threshold of B3/C4
    if ((songSettings.leadBaseScaleFreq < 300.0f && mtRNG() % 10 == 4) &&
        internalAudioBuffer.backBufferLength == internalAudioBuffer.length)
    {
        const float twelthRootOf2 = powf(2.0f, 1.0f / 12.0f);
        songSettings.leadBaseScaleFreq = songSettings.leadBaseScaleFreq * powf(twelthRootOf2, 12.0f);
        std::cout << "   > RNJesus wants to switch the lead up an octave \n";
    }
    else if ((songSettings.leadBaseScaleFreq > 300.0f && mtRNG() % 6 == 1) &&
        internalAudioBuffer.backBufferLength == internalAudioBuffer.length)
    {
        const float twelthRootOf2 = powf(2.0f, 1.0f / 12.0f);
        songSettings.leadBaseScaleFreq = songSettings.leadBaseScaleFreq * powf(twelthRootOf2, -12.0f);
        std::cout << "   > RNJesus wants to switch the lead down an octave \n";
    }

    // Select lead pattern
    int pickRandLeadPattern;
    // The following patterns don't sound great when repeated back to back
    if (mtRNG() % 2 == 0 && (
        songSettings.prevPatternLead != 15 &&
        songSettings.prevPatternLead != 10 &&
        songSettings.prevPatternLead != 7 &&
        songSettings.prevPatternLead != 11 &&
        songSettings.prevPatternLead != 20 ))
    {
        pickRandLeadPattern = songSettings.prevPatternLead;
        std::cout << "   > RNJesus wants the lead to play the same pattern as last time... \n";
    }
    else
    {
        pickRandLeadPattern = mtRNG() % 20;

        // discourage these
        if (pickRandLeadPattern == 2 ||
            pickRandLeadPattern == 7 ||
            pickRandLeadPattern == 9 || // borderline
            pickRandLeadPattern == 10 ||
            pickRandLeadPattern == 11 ||
            pickRandLeadPattern == 12 ||
            pickRandLeadPattern == 13 ||
            pickRandLeadPattern == 15 ||
            pickRandLeadPattern == 19) {
            //std::cout << "Selected discouraged lead pattern: " << pickRandLeadPattern << ". Rerolling...\n";
            pickRandLeadPattern = mtRNG() % 20;
        }


        // really discourage these
        if (pickRandLeadPattern == 2  ||
            pickRandLeadPattern == 7  ||
            pickRandLeadPattern == 11 ||
            pickRandLeadPattern == 12 ) {
            //std::cout << "Selected really discouraged lead pattern: " << pickRandLeadPattern << ". Rerolling...\n";
            pickRandLeadPattern = mtRNG() % 20;
        }

        // Some patterns don't work super well with slower BPM settings
        if (songSettings.BPM > 121 && pickRandLeadPattern == 1)
            pickRandLeadPattern = mtRNG() % 20;

        // this pattern sucks at slower speeds
        if (songSettings.BPM > 121 && pickRandLeadPattern == 8)
            pickRandLeadPattern = 0;

        // this pattern sucks with lofi
        if (songSettings.loFi == true && pickRandLeadPattern == 16)
            pickRandLeadPattern = mtRNG() % 20;
    }

    //DEBUG
    // pickRandLeadPattern = 12;
    //DEBUG

    std::cout << "Playing lead pattern: " << pickRandLeadPattern << "\n";

    // Here we go!
    int beatCount = 1;
    int barCount = 1;

    switch (pickRandLeadPattern) {
    case 0: // random through out, 8 halfnotes
    {
        for (int c = 0; c < leadBufLength; c += (songSettings.halfNoteLenBytes)) // 8 notes to bar
        {
            int chooseNote = mtRNG() % 8;
            float noteFreq = key.freqs[chooseNote];
            SafeLead(noteFreq, songSettings.halfNoteLenMS, qtrMag, leadBuf, c);

            if (beatCount == 8)
            {
                beatCount = 0;
                barCount++;
            }
            beatCount++;
        }
        break;
    }
    case 1: // play a half note, wait, play another half note.
    {
        for (int c = 0; c < leadBufLength; c += (songSettings.halfNoteLenBytes))
        {
            int chooseNote = mtRNG() % 8;
            float noteFreq = key.freqs[chooseNote];

            if (beatCount % 2 == 1)
            {
                SafeLead(noteFreq, songSettings.halfNoteLenMS, qtrMag, leadBuf, c);
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
                chooseNote = mtRNG() % 8;
                noteFreq = key.freqs[chooseNote];
                SafeLead(noteFreq, songSettings.halfNoteLenMS, qtrMag, leadBuf, c);
            }
            else if (noteCount % 3 == 0) // Play the old note
            {
                SafeLead(noteFreq, songSettings.halfNoteLenMS, qtrMag, leadBuf, c);
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
            int chooseNote = mtRNG() % 10; // chance of silence
            if (chooseNote < 8)
            {
                float noteFreq = key.freqs[chooseNote];
                SafeLead(noteFreq, songSettings.halfNoteLenMS, qtrMag, leadBuf, c);
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
            int chooseNote = mtRNG() % 8;
            float noteFreq = key.freqs[chooseNote];
            SafeLead(noteFreq, songSettings.noteLenMS, qtrMag, leadBuf, c);

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
            int chooseNote = mtRNG() % 8;
            float noteFreq = key.freqs[chooseNote];
            SafeLead(noteFreq, songSettings.noteLenMS, qtrMag, leadBuf, c);
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
                SafeLead(noteFreq, climbSpeed / audioSettings.bytesPerMS, qtrMag, leadBuf, c);
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
                Scale tempKey(OppositeKeyMode(songSettings.scaleType), key.freqs[5]);
                firstInterval = tempKey.freqs[0];
                thirdInterval = tempKey.freqs[2];
                fifthInterval = tempKey.freqs[4];
                seventhInterval = tempKey.freqs[6];

            }
            else if (noteCount % 6 == 0 && arpCount % 7 == 3 && !randMode)
            {
                Scale tempKey(songSettings.scaleType, key.freqs[3]);
                firstInterval = tempKey.freqs[0];
                thirdInterval = tempKey.freqs[2];
                fifthInterval = tempKey.freqs[4];
                seventhInterval = tempKey.freqs[6];
            }
            else if (noteCount % 6 == 0 && arpCount % 7 == 4 && !randMode)
            {

                Scale tempKey(songSettings.scaleType, key.freqs[4]);
                firstInterval = tempKey.freqs[0];
                thirdInterval = tempKey.freqs[2];
                fifthInterval = tempKey.freqs[4];
                seventhInterval = tempKey.freqs[6];
            }
            else if (noteCount % 6 == 0 && arpCount % 7 == 5 && !randMode)
            {
                Scale tempKey(songSettings.scaleType, key.freqs[0]);
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
                int chooseNote = mtRNG() % 8;
                float noteFreq = key.freqs[chooseNote];
                SafeLead(noteFreq, climbSpeed / audioSettings.bytesPerMS, qtrMag, leadBuf, c);
            }
            else {
                if (noteCount % 6 == 0)
                    SafeLead(firstInterval, songSettings.qtrNoteLenMS, qtrMag, leadBuf, c);
                else if (noteCount % 6 == 1)
                    SafeLead(thirdInterval, songSettings.qtrNoteLenMS, qtrMag, leadBuf, c);
                else if (noteCount % 6 == 2)
                    SafeLead(fifthInterval, songSettings.qtrNoteLenMS, qtrMag, leadBuf, c);
                else if (noteCount % 6 == 3)
                    SafeLead(seventhInterval, songSettings.qtrNoteLenMS, qtrMag, leadBuf, c);
                else if (noteCount % 6 == 4)
                    SafeLead(fifthInterval, songSettings.qtrNoteLenMS, qtrMag, leadBuf, c);
                else if (noteCount % 6 == 5)
                {
                    SafeLead(thirdInterval, songSettings.qtrNoteLenMS, qtrMag, leadBuf, c);
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
        for (int c = 0; c < leadBufLength; c += (songSettings.qtrNoteLenBytes)) // 16 notes to bar
        {
            int chooseNote = mtRNG() % 10;

            if (chooseNote < 8)
            {
                float noteFreq = key.freqs[chooseNote];
                SafeLead(noteFreq, songSettings.qtrNoteLenMS, qtrMag, leadBuf, c);
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
                int chooseNote = mtRNG() % 8;
                float noteFreq = key.freqs[chooseNote];
                SafeLead(noteFreq, songSettings.qtrNoteLenMS, qtrMag, leadBuf, c);
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
            int chooseNote = mtRNG() % 8;
            float noteFreq = key.freqs[chooseNote];
            SafeLead(noteFreq, leadBufLength / audioSettings.bytesPerMS, qtrMag, leadBuf, c);
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
                SafeLead(noteFreq, climbSpeed / audioSettings.bytesPerMS, qtrMag, leadBuf, c);
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
                Scale tempKey(OppositeKeyMode(songSettings.scaleType), key.freqs[5]);
                firstInterval = tempKey.freqs[0];
                thirdInterval = tempKey.freqs[2];
                fifthInterval = tempKey.freqs[4];
                seventhInterval = tempKey.freqs[6];

            }
            else if (noteCount % 6 == 0 && arpCount % 7 == 3 && !randMode)
            {
                Scale tempKey(songSettings.scaleType, key.freqs[3]);
                firstInterval = tempKey.freqs[0];
                thirdInterval = tempKey.freqs[2];
                fifthInterval = tempKey.freqs[4];
                seventhInterval = tempKey.freqs[6];
            }
            else if (noteCount % 6 == 0 && arpCount % 7 == 4 && !randMode)
            {

                Scale tempKey(songSettings.scaleType, key.freqs[4]);
                firstInterval = tempKey.freqs[0];
                thirdInterval = tempKey.freqs[2];
                fifthInterval = tempKey.freqs[4];
                seventhInterval = tempKey.freqs[6];
            }
            else if (noteCount % 6 == 0 && arpCount % 7 == 5 && !randMode)
            {
                Scale tempKey(songSettings.scaleType, key.freqs[0]);
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
                int chooseNote = mtRNG() % 8;
                float noteFreq = key.freqs[chooseNote];
                SafeLead(noteFreq, climbSpeed / audioSettings.bytesPerMS, qtrMag, leadBuf, c);
            }
            else {
                if (noteCount % 6 == 0)
                    SafeLead(firstInterval, songSettings.halfNoteLenMS, qtrMag, leadBuf, c);
                else if (noteCount % 6 == 1)
                    SafeLead(thirdInterval, songSettings.halfNoteLenMS, qtrMag, leadBuf, c);
                else if (noteCount % 6 == 2)
                    SafeLead(fifthInterval, songSettings.halfNoteLenMS, qtrMag, leadBuf, c);
                else if (noteCount % 6 == 3)
                    SafeLead(seventhInterval, songSettings.halfNoteLenMS, qtrMag, leadBuf, c);
                else if (noteCount % 6 == 4)
                    SafeLead(fifthInterval, songSettings.halfNoteLenMS, qtrMag, leadBuf, c);
                else if (noteCount % 6 == 5)
                {
                    SafeLead(thirdInterval, songSettings.halfNoteLenMS, qtrMag, leadBuf, c);
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

        for (int c = 0; c < leadBufLength; c += songSettings.barLenBytes)
        {

            int speed = 16;
            
            int newKeyDegree = mtRNG() % 7 + 1;

            // Prefer the 1st, 4th or 5th degree scales of the key.
            if (newKeyDegree == 2 || newKeyDegree == 3 || newKeyDegree == 7)
                newKeyDegree = mtRNG() % 7 + 1;

            std::pair<float, ScaleType> newArpFreqTypePair = GiveKeyScale(Notes.getNoteFreq(songSettings.keyNote + "3"), songSettings.keyType, newKeyDegree);

            Scale tempKey(newArpFreqTypePair.second, newArpFreqTypePair.first);

            GenArp(tempKey.notes["1st"], songSettings.noteLenMS, speed, qtrMag, leadBuf, c, false);

            if (beatCount == 1)
            {
                beatCount = 0;
                //std::cout << "bar Count: " << barCount << "\n";
                barCount++;
            }
            beatCount++;
        }
        break;
    }
    case 13: // 4 pre-randomised notes
    {
        float Note1 = key.freqs[mtRNG() % 8];
        float Note2 = key.freqs[mtRNG() % 8];
        float Note3 = key.freqs[mtRNG() % 8];
        float Note4 = key.freqs[mtRNG() % 8];

        for (int c = 0; c < leadBufLength; c += songSettings.halfNoteLenBytes)
        {

            if (mtRNG() % 3 == 0)
            {
                Note1 = key.freqs[mtRNG() % 8];
                Note2 = key.freqs[mtRNG() % 8];
                Note3 = key.freqs[mtRNG() % 8];
                Note4 = key.freqs[mtRNG() % 8];
            }

            if (mtRNG() % 5 != 0) {
                if (barCount == 1)
                    SafeLead(Note1, songSettings.halfNoteLenMS - 10, qtrMag, leadBuf, c);
                if (barCount == 2)
                    SafeLead(Note2, songSettings.halfNoteLenMS - 10, qtrMag, leadBuf, c);
                if (barCount == 3)
                    SafeLead(Note3, songSettings.halfNoteLenMS - 10, qtrMag, leadBuf, c);
                if (barCount == 4)
                    SafeLead(Note4, songSettings.halfNoteLenMS - 10, qtrMag, leadBuf, c);
            }

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
                int chooseNote = mtRNG() % 10;
                if (chooseNote < 8)
                {
                    float noteFreq = key.freqs[chooseNote];
                    SafeLead(noteFreq, songSettings.qtrNoteLenMS, qtrMag, leadBuf, c);
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
    }
    case 15: // second 2 bars, slide down x2
    {
        for (int c = 0; c < leadBufLength; c += (songSettings.barLenBytes))
        {

            if (barCount == 4)
            {
                SlideSquare(key.freqs[7], key.freqs[0], songSettings.barLenMS, qtrMag, leadBuf, c);
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
    case 16: // random through out, 4 notes per bar, sliding to notes
    {
        float noteFreq = 0.0f;
        int chooseNote = mtRNG() % 8;
        float lastNoteFreq = key.freqs[chooseNote];

        for (int c = 0; c < leadBufLength; c += (songSettings.noteLenBytes)) // 4 beats
        {

            if (barCount == 1 && beatCount == 1)
            {
                chooseNote = mtRNG() % 8;
                noteFreq = key.freqs[chooseNote];
                SlideSquare(lastNoteFreq, noteFreq, songSettings.noteLenMS, qtrMag, leadBuf, c);
                lastNoteFreq = noteFreq;
            }
            else if (beatCount % 2 == 0)
            {
                SafeSquare(noteFreq, songSettings.noteLenMS, qtrMag, leadBuf, c);
                lastNoteFreq = noteFreq;

            }
            else
            {
                chooseNote = mtRNG() % 8;
                noteFreq = key.freqs[chooseNote];
                SlideSquare(lastNoteFreq, noteFreq, songSettings.noteLenMS, qtrMag, leadBuf, c);
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
        int chooseNote = mtRNG() % 8;
        float freq1 = key.freqs[chooseNote];
        float freq2;

        do {
            chooseNote = mtRNG() % 8;
            freq2 = key.freqs[chooseNote];
        } while (freq2 == freq1);

        float freq3;
        do {
            chooseNote = mtRNG() % 8;
            freq3 = key.freqs[chooseNote];
        } while (freq3 == freq1 || freq3 == freq2);

        for (int c = 0; c < leadBufLength; c += (songSettings.noteLenBytes)) // 4 beats
        {
            chooseNote = mtRNG() % 4;
            if (chooseNote == 0)
            {
                SafeLead(freq1, songSettings.noteLenMS, qtrMag, leadBuf, c);
                //SafeFadeOut(leadBuf, songSettings.noteLenBytes, c);
            }
            else if (chooseNote == 1)
            {
                SafeLead(freq2, songSettings.noteLenMS, qtrMag, leadBuf, c);
                // SafeFadeOut(leadBuf, songSettings.noteLenBytes, c);
            }
            else if (chooseNote == 2)
            {
                SafeLead(freq3, songSettings.noteLenMS, qtrMag, leadBuf, c);
                //SafeFadeOut(leadBuf, songSettings.noteLenBytes, c);
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
    case 18: // random through out, 8 notes per bar
    {
        float noteFreq;
        for (int c = 0; c < leadBufLength; c += (songSettings.halfNoteLenBytes))
        {
            int chooseNote = mtRNG() % 10;
            if (chooseNote < 8)
                noteFreq = key.freqs[chooseNote];
            else
                noteFreq = 1;
           
            if (beatCount % 2 == 1 && noteFreq > 1)
            {
                SafeLead(noteFreq, songSettings.halfNoteLenMS, qtrMag, leadBuf, c);
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
    case 19: // alternate between two notes, change after 2 bars
    {
        int chooseNote = mtRNG() % 8;
        float freq1 = key.freqs[chooseNote];
        chooseNote = mtRNG() % 8;
        float freq2 = key.freqs[chooseNote];

        for (int c = 0; c < leadBufLength; c += (songSettings.halfNoteLenBytes)) // 8 beats
        {

            if (beatCount % 2 == 1)
                SafeLead(freq1, songSettings.halfNoteLenMS, qtrMag, leadBuf, c);
            else
                SafeLead(freq2, songSettings.halfNoteLenMS, qtrMag, leadBuf, c);


            if (barCount == 3 && beatCount == 1) {
                chooseNote = mtRNG() % 8;
                freq1 = key.freqs[chooseNote];
                chooseNote = mtRNG() % 8;
                freq2 = key.freqs[chooseNote];
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
    case 20: // random through out, 4 notes per bar, sliding all over the place
    {
        float noteFreq = 0.0f;
        int chooseNote = mtRNG() % 8;
        float lastNoteFreq = key.freqs[chooseNote];

        for (int c = 0; c < leadBufLength; c += (songSettings.dblNoteLenBytes)) // 4 beats
        {

            if (barCount == 1 && beatCount == 1)
            {
                chooseNote = mtRNG() % 8;
                noteFreq = key.freqs[chooseNote];

                if (noteFreq == lastNoteFreq) {
                    chooseNote = mtRNG() % 8;
                    noteFreq = key.freqs[chooseNote];
                }

                SlideSquare(lastNoteFreq, noteFreq, songSettings.dblNoteLenMS, qtrMag, leadBuf, c);
                lastNoteFreq = noteFreq;
            }
            else if (beatCount % 2 == 0)
            {
                SafeSquare(noteFreq, songSettings.dblNoteLenMS, qtrMag, leadBuf, c);
                lastNoteFreq = noteFreq;

            }
            else
            {
                chooseNote = mtRNG() % 8;
                noteFreq = key.freqs[chooseNote];
                SlideSquare(lastNoteFreq, noteFreq, songSettings.dblNoteLenMS, qtrMag, leadBuf, c);
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
    default:
        break;
    }

    songSettings.prevPatternLead = pickRandLeadPattern;


#ifdef DEBUG_AUDIO
    DumpBuffer(leadBuf, leadBufLength, "LeadBuffer.txt");
#endif
}

