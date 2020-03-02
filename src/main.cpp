#include "MusicGen.h"

// main etc.
int main(int argc, char* argv[])
{
    srand(0);
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        std::cout << "Could not initialise SDL";
    }
    else
    {
        std::cout << "SDL initialised!!\n";
    }

    Menu();

    return 0;
}


void Menu()
{
    while (true) {

		std::cout << "BPM: " << songSettings.BPM << "\n";
        std::cout << "Key/Scale: " << songSettings.keyNote << " " << (songSettings.key == Key::Major ? "Major" : "Minor") << "\n";
        std::cout << "LoFi: " << (songSettings.hiFi == true ? "Yep" : "Nup") << "\n";
#ifdef DEBUG_AUDIO
        std::cout << "noteLength(ms): " << songSettings.noteLenMS << "\n";
		std::cout << "songSettings.barLenMS: " << songSettings.barLenMS << "\n";
		std::cout << "beatsToBar: " << songSettings.beatsToBar << "\n";
#endif
        if (SDL_GetError() == NULL)
        {
            printf("SDL Error from last command (if any): %s\n", SDL_GetError());
        }

        std::cout << "\n\n"
            "Press c to change settings\n"
            "Press s to start music generator\n"
            "Press P to pause music generation\n"
            "Press o to test old wave/noise/effects generators\n"
            "Press d to test drums\n"
            "Press e to play a major scale\n"
            "Press t to test wave/noise/effect generators with new method\n"
            "Press q to quit\n\nAnswer: ";

        char menuInput = std::cin.get();

        if (menuInput == 'c')
            ChangeAudioSettings();
        else if (menuInput == 's')
            SetupAudio(true);
        else if (menuInput == 'p')
            audioSettings.StopAudio();
        else if (menuInput == 'o')
            DebugGenerators();
        else if (menuInput == 'd')
            TestDrums();
        else if (menuInput == 'e')
            PlayScale();
        else if (menuInput == 't')
            DebugGeneratorsNew();
        else if (menuInput == 'q')
        {
            SDL_Quit();
            exit(0);
        }
        std::cin.get();
    }
}

void ChangeAudioSettings()
{
    std::cout << "Unimplemented\n";
}
