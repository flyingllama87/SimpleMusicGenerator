//
//  TestFunctions.cpp
//  test
//
//  Created by Morgan on 28/2/20.
//  Copyright © 2020 Morgan. All rights reserved.
//

#include "MusicGen.h"


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

#ifndef _WIN32 || _WIN64
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

#ifndef _WIN32 || _WIN64
    std::cout << "Current path is " << std::__fs::filesystem::current_path() << '\n';
#endif
}

