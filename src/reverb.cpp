
// Taken from https://github.com/SergeiGorskiy/IFMO_Reverb.git
// Modified for Single Channel
// Cruft Removed
// Move to secure functions where easy
// Comments translated by Google Translate

#define META 256 // 44

#define _CRT_SECURE_NO_WARNINGS

#ifndef DISABLE_REVERB

#include <string.h>
#include <stdlib.h>
#include "reverb.h"

void Reverb(short* inL, short* outL, int bufLen) {

    for (int i = 0; i < bufLen / 2; i++) {

        int pos = META + i;
        short outputL = 0, inputL;

        inputL = (short)(*(inL + pos) * gain);

        // each time the filters are passed:
        // 1) a very muffled raw sample
        // 2) index of the desired buffer (for arrays of iterators and sizes)
        // 3) pointer to the buffer itself

        for (short i = 0; i < 8; ++i) {
            outputL += combFunc(inputL, 2 * i, comb[2 * i]);
        }

        for (short i = 0; i < 4; ++i) {
            outputL = allpassFunc(outputL, 2 * i, allpass[2 * i]);
        }

        *(outL + pos) = outputL;
    }
}


// comb filter
short combFunc(short sample, short bufId, short* bufP) {

    short delayedSmpl = 0,
        delayedRflctn = reflection;

    // delayed (previous) sample - a sample that was played by combSize [bufId] samples back
    delayedSmpl = *(bufP + combItr[bufId]);

    // reflected sound = slightly muted delayed sample + strongly muted delayed reflection
    reflection = (short)(delayedSmpl * resonance2) + (short)(delayedRflctn * resonance);

    // current sample = raw sample + reflected sound
    *(bufP + combItr[bufId]) = (short)((float)(sample)+(reflection * reflectionLvl));

    if (++combItr[bufId] >= combSize[bufId])
        combItr[bufId] = 0;

    return delayedSmpl;
}

// phase filter
short allpassFunc(short filteredSmpl, short bufId, short* bufP) {

    short reverbedSmpl,
        delaydFiltSmpl;
    float feedback = 0.5;

    // delayed processed sample - processed sample that sounded allpassSize [bufId] samples back
    delaydFiltSmpl =* (bufP + allpassItr[bufId]);

    // ready sample = inverted processed sample + delayed processed sample
    reverbedSmpl = -filteredSmpl + delaydFiltSmpl;

    // current processed sample = processed sample + muted delayed processed sample
    *(bufP + allpassItr[bufId]) = (short)((float)(filteredSmpl)+(delaydFiltSmpl * feedback));

    if (++allpassItr[bufId] >= allpassSize[bufId])
        allpassItr[bufId] = 0;

    return reverbedSmpl;
}

char format_name[7],
quality[27];

unsigned char buffer_4[4],
buffer_2[2];

short stereospread = 23,
reflection = 0;

float resonance,
resonance2,
initialResonance,
scaleResonance;

float reflectionLvl,
scaleRoom = 0.28,
offsetRoom = 0.5,
initialRoom,
gain = 0.03;

short combSize[16] = { 1116, 1116 + 23, 1188, 1188 + 23, // buffer sizes
                      1277, 1277 + 23, 1356, 1356 + 23,
                      1422, 1422 + 23, 1488, 1488 + 23,
                      1557, 1557 + 23, 1617, 1617 + 23 };

short allpassSize[8] = { 556, 556 + 23, 441, 441 + 23,
                        341, 341 + 23, 228, 228 + 23 };

short combItr[16] = {// iterators for each buffer
        0,0,0,0,
        0,0,0,0,
        0,0,0,0,
        0,0,0,0
};

short allpassItr[8] = {
        0,0,0,0,
        0,0,0,0
};


void InitReverb() {

    gain = 0.03; // 0.03
    initialRoom = 0.8; // 0.7
    initialResonance = 0.6; // 0.5

    short* arr1[16] = {// initialization of buffers
            L1, R1,
            L2, R2,
            L3, R3,
            L4, R4,
            L5, R5,
            L6, R6,
            L7, R7,
            L8, R8
    };

    short* arr2[8] = {
            aL1, aR1,
            aL2, aR2,
            aL3, aR3,
            aL4, aR4
    };

    *comb = *arr1;
    *allpass = *arr2;

    short* tmpP = 0;
    for (int i = 0; i < 16; ++i) {
        comb[i] = allocBuffer(tmpP, combSize[i]);
    }

    for (int i = 0; i < 8; ++i) {
        allpass[i] = allocBuffer(tmpP, allpassSize[i]);
    }

    setResonance();
    reflectionLvl = (initialRoom * scaleRoom) + offsetRoom;
}

void setResonance() {
    resonance = initialResonance * scaleResonance;
    resonance2 = 1 - resonance;
};

short* allocBuffer(short* buf, int size) {
    buf = (short*)(calloc(size, sizeof(buf)));

    if (!buf) {
        printf("Error allocating memory \n");
        exit(1);
    }
    else return buf;
}

void removeBuffers(short** bufArray) {
    for (int i = 0; i < sizeof(bufArray); ++i)
        free(*(bufArray + i));
}

#endif