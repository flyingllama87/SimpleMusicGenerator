//
//  MusicGenInterface.cpp
//  SimpleMusicGen-ios
//
//  Created by Morgan on 12/3/20.
//  Copyright © 2020 Morgan Robertson. All rights reserved.
//

#include "MusicGen.h"


extern "C" void SetupSDL()
{
    InitSDL();
}

extern "C" void PlayAudio()
{
    SetupAudio(true);
}

extern "C" void StopAudio()
{
    audioSettings.StopAudio();
}
