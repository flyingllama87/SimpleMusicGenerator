//
//  wrapper.cpp
//  MusicGen-iOS
//
//  Created by Morgan on 11/3/20.
//  Copyright © 2020 Morgan Robertson. All rights reserved.
//

#include "MusicGen.h"


extern "C" int getIntFromCPP()
{
    // Create an instance of A, defined in
    // the library, and call getInt() on it:
    return A(1234).getInt();
}
