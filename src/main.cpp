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
		std::cout << "\nBPM: " << songSettings.BPM << "\n";
        std::cout << "Key/Scale: " << songSettings.keyNote << " " << (songSettings.key == Key::Major ? "Major" : "Minor") << "\n";
        std::cout << "LoFi: " << (songSettings.loFi == true ? "Yep" : "Nup") << "\n";
#ifdef DEBUG_AUDIO
        std::cout << "noteLength(ms): " << songSettings.noteLenMS << "\n";
		std::cout << "songSettings.barLenMS: " << songSettings.barLenMS << "\n";
		std::cout << "beatsToBar: " << songSettings.beatsToBar << "\n";
#endif
        if (SDL_GetError() == NULL)
        {
            printf("SDL Error from last command (if any): %s\n", SDL_GetError());
        }

        std::cout << "\n"
            "Press c to change settings\n"
            "Press s to start music generator\n"
            "Press P to pause music generation\n"
            "Press o to test old wave/noise/effects generators\n"
            "Press d to test drums\n"
            "Press e to play a major scale\n"
            "Press t to test wave/noise/effect generators with new method\n"
            "Press q to quit\n\nAnswer: ";

        std::string menuStr;
        std::getline(std::cin, menuStr);
        char menuInput = menuStr[0];

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
    }
}

void ChangeAudioSettings()
{
    bool goBack = false;
    
    while (!goBack)
    {
        std::cout << "BPM: " << songSettings.BPM << "\n"
        "Key/Scale: " << songSettings.keyNote << " " << (songSettings.key == Key::Major ? "Major" : "Minor") << "\n"
        "LoFi: " << (songSettings.loFi == true ? "Yep" : "Nup") << "\n\n";
        
        std::cout << "What do you want to change?\n"
            "Press 1 to change BPM\n"
            "Press 2 to change key base note\n"
            "Press 3 to switch key tonality to major / minor\n"
            "Press 4 to change fidelity\n"
            "Press 5 to go back\n";
        
        std::string selection;
        std::getline(std::cin, selection);
        
        switch (selection[0]) {
            case '1':
            {
                while (true)
                {
                    std::string strInputBPM;
                    std::cout << "\nWhat do you want to set the BPM to? Needs to be divisible by 4. \n Answer: ";
                    std::getline(std::cin, strInputBPM);
                    if (!stringIsInt(strInputBPM))
                    {
                        std::cout << "Invalid input\n";
                        continue;
                    }
                    int inputBPM = std::stoi(strInputBPM);
                    if (inputBPM % 4)
                    {
                        std::cout << "Not divisible by 4!\n";
                        continue;
                    }
                    if (inputBPM > 240 || inputBPM == 0)
                    {
                        std::cout << "Too fast!\n";
                        continue;
                    }
                    songSettings.BPM = inputBPM;
                    break;
                }
                break;
            }
            case '2':
            {
                while (true)
                {
                    std::string strInputNote;
                    std::cout << "\nWhat base note do you want to use? ";
                    std::getline(std::cin, strInputNote);
                    if (!IsANote(strInputNote))
                    {
                        std::cout << "Invalid input\n";
                        continue;
                    }
                    songSettings.keyNote = strInputNote[0];
                    break;
                }
                break;
            }
            case '3':
            {
                if (songSettings.key == Key::Major)
                    songSettings.key = Key::Minor;
                else
                    songSettings.key = Key::Major;
                break;
            }
            case '4':
            {
                if (songSettings.loFi == true)
                {
                    songSettings.loFi = false;
                    audioSettings.audSpecWant.freq = 48000;
                    audioSettings.audSpecWant.samples = 32768;
                }
                else
                {
                    songSettings.loFi = true;
                    audioSettings.audSpecWant.freq = 8000;
                    audioSettings.audSpecWant.samples = 4096;
                }
                break;
            }
            case '5':
            {
                goBack = true;
                break;
            }
            default:
                break;
        }
    }
}
