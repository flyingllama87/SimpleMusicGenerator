
#include <stdio.h>

extern short   stereospread,
reflection;

extern short   combSize[16],
allpassSize[8],
combItr[16],
allpassItr[8];

extern float   resonance,
resonance2,
initialResonance,
scaleResonance;

extern float   reflectionLvl,
scaleRoom,
offsetRoom,
initialRoom,
gain;

short* L1, * R1,
* L2, * R2,
* L3, * R3,
* L4, * R4,
* L5, * R5,
* L6, * R6,
* L7, * R7,
* L8, * R8;

short* aL1, * aR1,
* aL2, * aR2,
* aL3, * aR3,
* aL4, * aR4;

short* comb[16], // array of pointers to 16 buffers for comb filters
* allpass[8]; // array of pointers to 8 buffers for the phase filter


void InitReverb();
void setResonance();
short* allocBuffer(short*, int);
void removeBuffers(short**);
short combFunc(short, short, short*);
short allpassFunc(short, short, short*);

