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
    std::ifstream wordList("wordlist.txt");
    // Seed random number gen in background thread
    for (int c = 0; c < randLineNum; c++)
    {
        getline(wordList, word);
    }
    return word;
}
