//
//  UtilFunctions.cpp
//
//  Created by Morgan on 28/2/20.
//  Copyright © 2020 Morgan. All rights reserved.
//

#include "MusicGen.h"
#include <fstream>

#ifdef _WIN64
#include <Windows.h>
#endif

// Helper functions

// Convert int16_t array to Uint8 little endian
void c16to8(int16_t* inBuf, int len, Uint8* outBuf)
{
    for (auto c = 0; c < len; c++)
    {
        outBuf[c * 2] = inBuf[c] & 0xFF;
        outBuf[(c * 2) + 1] = inBuf[c] >> 8;
    }
}

void DumpBuffer(Uint8* wavBuffer, int length, std::string fileName)
{
    std::ofstream waveBufferFile(fileName.c_str(), std::ios::out | std::ios::binary);
    for (int c = 0; c < length; c++)
    {
        waveBufferFile.write(reinterpret_cast<const char*>(&wavBuffer[c]), sizeof wavBuffer[c]);
    }
    waveBufferFile.close();

#ifdef PRINT_PATHS
    std::cout << "Current path is " << std::__fs::filesystem::current_path() << '\n';
#endif
}

void DumpBuffer(int16_t* wavBuffer, int length, std::string fileName)
{
    std::ofstream waveBufferFile(fileName.c_str(), std::ios::out | std::ios::binary);
    for (int c = 0; c < length; c++)
    {
        waveBufferFile.write(reinterpret_cast<const char*>(&wavBuffer[c]), sizeof wavBuffer[c]);
    }
    waveBufferFile.close();

#ifdef PRINT_PATHS
    std::cout << "Current path is " << std::__fs::filesystem::current_path() << '\n';
#endif
}


bool StringIsFloat (const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && (std::isdigit(*it) || *it == '.' )) ++it;
    return !s.empty() && it == s.end();
}

bool StringIsInt (const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

bool IsANote(std::string str)
{
    if (int(str[0]) >= 'A' && int(str[0]) <= 'G')
        return true;
    else
        return false;
}

// Only copy from the source buffer if there is enough room in the internal audio buffers
void SafeMemCopy(Uint8* destBuf, Uint8* srcBuf, Uint32 srcBufLen, int c, int destBufLen)
{
    if (srcBufLen > destBufLen - c)
        srcBufLen = destBufLen - c;
        
    memcpy(&destBuf[c], srcBuf, srcBufLen);

}


// This generates a number by summing together the numeric values of all characters in a word.
unsigned WordToNumber(std::string word)
{
    unsigned num = 0;

    for (int c = 0; c < word.length(); c++)
    {
        num += (int)word[c];
    }
    return num * word.length();
}

// This selects a random word from the word list.
std::string RandomWordFromWordList()
{
    int linesInFile = 7776;
    int randLineNum = rand() % linesInFile;
    std::string word = "";
    std::cout << "loading word list\n";
    std::ifstream wordList("wordlist.txt");
    std::cout << "loaded word list\n";
    for (int c = 0; c < randLineNum; c++)
    {
        getline(wordList, word);
    }
    return word;
}

void StatusCheck()
{

    std::cout << 
    "\n\n** STATUS CHECK ** \n\n"
    "audioSettings.inited: " << audioSettings.inited << "\n"
    "audioSettings.audSpecWant.freq: " << audioSettings.audSpecWant.freq << "\n"
    "audioSettings.audSpecHave.freq: " << audioSettings.audSpecHave.freq << "\n"
    "audioSettings.samplesPerMS: " << audioSettings.samplesPerMS << "\n"
    "audioSettings.bytesPerMS: " << audioSettings.bytesPerMS << "\n"
    "Bit Size:" << SDL_AUDIO_BITSIZE(audioSettings.audSpecHave.format) << "\n";

    switch (SDL_GetAudioDeviceStatus(audioSettings.device))
    {
    case SDL_AUDIO_STOPPED: std::cout << "audioSettings.device Status: stopped\n"; break;
    case SDL_AUDIO_PLAYING: std::cout << "audioSettings.device Status: playing\n"; break;
    case SDL_AUDIO_PAUSED: std::cout << "audioSettings.device Status: paused\n"; break;
    default: std::cout << "audioSettings.device Status: unknown status\n"; break;
    }

    std::cout << 
    "internalAudioBuffer.inited: " << internalAudioBuffer.inited << "\n"
    "internalAudioBuffer.pos: " << internalAudioBuffer.pos << "\n"
    "internalAudioBuffer.length: " << internalAudioBuffer.length << "\n"
    "internalAudioBuffer.backBufferLength: " << internalAudioBuffer.backBufferLength << "\n";

    // "internalAudioBuffer.buf" << internalAudioBuffer.buf == null ? 1 : 0 <<
    // "internalAudioBuffer.backBuf" << internalAudioBuffer.backBuf<< "\n";

    std::cout <<
    "songSettings.inited: " << songSettings.inited << "\n"
    "songSettings.rngSeedString: " << songSettings.rngSeedString << "\n"
    "songSettings.rngSeed: " << songSettings.rngSeed << "\n"
    "songSettings.BPM: " << songSettings.BPM << "\n"
    "songSettings.beatsToBar: " << songSettings.beatsToBar << "\n"
    "songSettings.barsPerMin: " << songSettings.barsPerMin << "\n"
    "songSettings.loFi: " << songSettings.loFi << "\n"
    "songSettings.drumsInited: " << songSettings.drumsInited << "\n"
    "songSettings.sectionCount: " << songSettings.sectionCount << "\n"
    "songSettings.prevPatternDrums: " << songSettings.prevPatternDrums << "\n"
    "songSettings.prevPatternBass: " << songSettings.prevPatternBass << "\n"
    "songSettings.prevPatternLead: " << songSettings.prevPatternLead << "\n";

}