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

    std::cout << "BPM: " << songSettings.BPM << "\n";
    std::cout << "noteLength(ms): " << songSettings.noteLenMS << "\n";
#ifdef DEBUG
	std::cout << "songSettings.barLenMS: " << songSettings.barLenMS << "\n";
	std::cout << "beatsToBar: " << songSettings.beatsToBar << "\n";
#endif

    Menu();

    return 0;
}


void Menu()
{
    while (true) {
        
        printf("SDL Error from last command (if any): %s\n", SDL_GetError());

        std::cout << "\n\n"
            "Press a to setup audio without callback\n"
            "Press b to setup audio with callback\n"
            "Press c to test wave/noise/effects generators\n"
            "Press d to test drums\n"
            "Press e to play a major scale\n"
            "Press f to test wave/noise/effect generators with newer, more efficient method\n"
            "Press q to quit\n\nAnswer: ";

        char menuInput = std::cin.get();

        if (menuInput == 'a')
            SetupAudio();
        else if (menuInput == 'b')
            SetupAudio(true);
        else if (menuInput == 'c')
            DebugGenerators();
        else if (menuInput == 'd')
            TestDrums();
        else if (menuInput == 'e')
            PlayScale();
        else if (menuInput == 'f')
            DebugGeneratorsNew();
        else if (menuInput == 'q')
        {
            SDL_Quit();
            exit(0);
        }
        std::cin.get();
    }
}
