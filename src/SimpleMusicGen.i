/* File: SimpleMusicGen.i */
%module SimpleMusicGen

%{
#define SWIG_FILE_WITH_INIT
#include "MusicGen.h"
%}

void SetupAudio(bool callback);
int Init_SDL();