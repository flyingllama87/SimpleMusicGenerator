//
//  AudioAndMusic.cpp
//  test
//
//  Created by Morgan on 28/2/20.
//  Copyright © 2020 Morgan. All rights reserved.
//

#include "MusicGen.h"

struct audioSettings audioSettings;
struct songSettings songSettings;
struct internalAudioBuffer internalAudioBuffer;


struct Scale
{
	const float twelthRootOf2 = powf(2.0f, 1.0f / 12.0f);

	float keyFreq = 0.0f;

	std::map<std::string, float> notes;
	std::vector<float> freqs;
    
    Scale(Key key, float freq)
    {
        if (key == Key::Major)
            Major(freq);
        else
            Minor(freq);
    }
    
    // Disable default constructor
    Scale() = delete;
    
	void Major(float freq)
	{
		this->keyFreq = freq;

		notes.insert(std::make_pair("1st", this->keyFreq));
		notes.insert(std::make_pair("2nd", this->keyFreq * powf(twelthRootOf2, 2.0f)));
		notes.insert(std::make_pair("3rd", this->keyFreq * powf(twelthRootOf2, 4.0f)));
		notes.insert(std::make_pair("4th", this->keyFreq * powf(twelthRootOf2, 5.0f)));
		notes.insert(std::make_pair("5th", this->keyFreq * powf(twelthRootOf2, 7.0f)));
		notes.insert(std::make_pair("6th", this->keyFreq * powf(twelthRootOf2, 9.0f)));
		notes.insert(std::make_pair("7th", this->keyFreq * powf(twelthRootOf2, 11.0f)));
		notes.insert(std::make_pair("8th", this->keyFreq * powf(twelthRootOf2, 12.0f)));

		for (std::map<std::string, float>::iterator it = notes.begin(); it != notes.end(); it++)
		{
			freqs.push_back(it->second);
		}
	}
    
    void Minor(float freq)
    {
        this->keyFreq = freq;

        notes.insert(std::make_pair("1st", this->keyFreq));
        notes.insert(std::make_pair("2nd", this->keyFreq * powf(twelthRootOf2, 2.0f)));
        notes.insert(std::make_pair("3rd", this->keyFreq * powf(twelthRootOf2, 3.0f)));
        notes.insert(std::make_pair("4th", this->keyFreq * powf(twelthRootOf2, 5.0f)));
        notes.insert(std::make_pair("5th", this->keyFreq * powf(twelthRootOf2, 7.0f)));
        notes.insert(std::make_pair("6th", this->keyFreq * powf(twelthRootOf2, 8.0f)));
        notes.insert(std::make_pair("7th", this->keyFreq * powf(twelthRootOf2, 10.0f)));
        notes.insert(std::make_pair("8th", this->keyFreq * powf(twelthRootOf2, 12.0f)));

        for (std::map<std::string, float>::iterator it = notes.begin(); it != notes.end(); it++)
        {
            freqs.push_back(it->second);
        }
    }
};


// len is number of bytes not number of samples requested.
void GenAudioStream(void* userdata, Uint8* stream, int len)
{
    /*
    int noOfSamplesRequested = len / 2;

    std::cout << "No. of bytes requested: " << len << "\nLength of audio requested (ms): " << noOfSamplesRequested / audioSettings.samplesPerMS << "\n";
    std::cout << "No. of samples requested: " << len / sizeof(int16_t) << "\n";
    
	
    std::cout << "\nIn gen func.\n"
    "No of samples requested: " << noOfSamplesRequested << ".\n"
    "Samples per ms: " << audioSettings.samplesPerMS << ".\n"
    "ms of audio requested: " << (float)noOfSamplesRequested / audioSettings.samplesPerMS << "\n";
    std::cout << "internalAudioBuffer.pos: " << internalAudioBuffer.pos << "\n\n";
	*/

    int bytesTillIntBufEnd = internalAudioBuffer.length - internalAudioBuffer.pos;
    if (!audioSettings.inited || !songSettings.inited || !internalAudioBuffer.inited)
    {
        std::cout << "Skipping callback as music functionality is not set up";
    }
    else if (internalAudioBuffer.pos == -1) // Fill audio buffer on first run.
    {
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
        // Generate a new drum beat / music track
        GenMusicStream();
        // fill the stream buffer with whatever is required from the NEW drum buffer
        memcpy(&stream[bytesTillIntBufEnd], &internalAudioBuffer.buf[internalAudioBuffer.pos], extraBytesRequired);
        internalAudioBuffer.pos += extraBytesRequired;
    }
    else // We still have data in the internal buffer to use
    {
        memcpy(stream, &internalAudioBuffer.buf[internalAudioBuffer.pos], len);
        internalAudioBuffer.pos += len;
    }

#ifdef DEBUG_AUDIO
    DumpBuffer(stream, len, "ReqBuf8.txt");
#endif
}


void GenMusicStream()
{
    // Clear internal buffer
    std::fill_n(internalAudioBuffer.buf, internalAudioBuffer.length, 0);

    // Drums

    Uint8* drumBuf = new Uint8[internalAudioBuffer.length] ();
    GenDrumBeat(drumBuf);

    Uint8* bassBuf = new Uint8[internalAudioBuffer.length] ();
    GenBassTrack(bassBuf);

    internalAudioBuffer.pos = 0;

    SDL_MixAudioFormat(internalAudioBuffer.buf, drumBuf, sampleFmt, internalAudioBuffer.length, SDL_MIX_MAXVOLUME);
    SDL_MixAudioFormat(internalAudioBuffer.buf, bassBuf, sampleFmt, internalAudioBuffer.length, SDL_MIX_MAXVOLUME);

    delete[] drumBuf;
    delete[] bassBuf;

}

void GenBassTrack(Uint8* bassBuf)
{
    // Get base note for bass
    std::string bassScaleNote;
    bassScaleNote.append(songSettings.keyNote);
    bassScaleNote.append("2"); // second octave for bass
    
#ifdef DEBUG_AUDIO
    std::cout << "Bass base scale note: " << bassScaleNote << "\n";
#endif
    
    // Construct key scale
    Scale key(songSettings.key, Notes.getNoteFreq(bassScaleNote));
    
    // Select bass pattern
    int pickRandBassPattern = rand() % 7;
    std::cout << "Playing bass pattern: " << pickRandBassPattern << "\n\n";

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
            Sawtooth(noteFreq, songSettings.halfNoteLenMS, qtrMag, &bassBuf[c]);

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
			Sawtooth(noteFreq, songSettings.barLenMS, qtrMag, &bassBuf[c]);

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
				Sawtooth(noteFreq, songSettings.halfNoteLenMS, qtrMag, &bassBuf[c]);
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
				Sawtooth(noteFreq, songSettings.noteLenMS, qtrMag, &bassBuf[c]);
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
            Sawtooth(noteFreq, songSettings.halfNoteLenMS, qtrMag, &bassBuf[c]);

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
			Sawtooth(noteFreq, songSettings.noteLenMS, qtrMag, &bassBuf[c]);

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
			Sawtooth(noteFreq, songSettings.noteLenMS, qtrMag, &bassBuf[c]);
			FadeOut(&bassBuf[c], songSettings.noteLenBytes);

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
    DumpBuffer(bassBuf, internalAudioBuffer.length, "BassBuffer,txt");
#endif
}

void PlayScale()
{
    SetupAudio();

    std::string strInputNoteName;
    float fInputFreq;
    AudioData scale;

    // Buffer for total scale (8 notes) in 16-bit format
    int bufLen16 = songSettings.noteLenMS * audioSettings.samplesPerMS * 8;
    // Buffer for total scale (8 notes) in 8-bit format
    int bufLen = bufLen16 * 2;
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
