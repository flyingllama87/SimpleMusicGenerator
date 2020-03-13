//
//  Use this file to import your target's public headers that you would like to expose to Swift.
//

#include "stdbool.h"

struct userSettings {
    int BPM;
    char note;
    int scale;
    bool lofi;
};

void SetupSDL();
void PlayAudio();
void StopAudio();
void SongConfig(int bpm, char note, int scale, bool lofi);
struct userSettings getSongSettings();
