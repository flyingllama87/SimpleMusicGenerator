#include "MusicGen.h"

#ifdef _WIN64
#include "windows.h"
#endif

#include <chrono>

// Menu / CLI specific
void SetSeedAndPlay();
void Menu();
void ChangeSongSettingsCLI(); // CLI for changing song & audio settings.

// main etc.
int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
        std::cout << "Could not initialise SDL";
    else
        std::cout << "SDL initialised!!\n";

    // Seed random number gen in main thread
    srand(std::chrono::system_clock::now().time_since_epoch().count());

    Menu();

    return 0;
}


void Menu()
{
    while (true) {

        if (SDL_GetError() == NULL)
        {
            std::cout << "SDL Error from last command (if any):" << SDL_GetError() << "\n";
        }

        std::cout << "\n"
            " --- MAIN MENU ---\n\n"
            "Type a letter and press enter:\n\n"
            " s to start music generator with configured settings\n"
            " c to change settings\n"
            " r to start music generator with random seed word\n"
            " w to set a seed word and play\n"
            " p to pause music generation\n"
            " q to quit\n\n"
#ifdef ENABLE_DEBUG_FUNCTIONS
            " --- DEBUG FUNCTIONS ---\n\n"
            " t to test wave/noise/effects generators\n"
            " d to test drums\n"
            " f to test sliding square waves\n"
            " m to play a major scale\n"
            " n to play a minor scale\n"
            " l to test selecting a random seed\n"
            " a to test arpeggios\n"
            " k to test scale/chord progression\n\n"
#endif
            ;

        std::cout << " --- CURRENT SETTINGS ---\n\n";
        std::cout << " BPM: " << songSettings.BPM << "\n";
        std::cout << " Key/Scale: " << songSettings.keyNote << " " << (songSettings.scaleType == ScaleType::Major ? "Major" : "Minor") << "\n";
        std::cout << " LoFi: " << (songSettings.loFi == true ? "Yep" : "Nup") << "\n";
        std::cout << " Seed word: " << songSettings.rngSeedString << "\n";
#ifdef DEBUG_AUDIO
        std::cout << " noteLength(ms): " << songSettings.noteLenMS << "\n";
        std::cout << " songSettings.barLenMS: " << songSettings.barLenMS << "\n";
        std::cout << " beatsToBar: " << songSettings.beatsToBar << "\n";
#endif

        std::string menuStr;
        std::getline(std::cin, menuStr);
        char menuInput = menuStr[0];

        if (menuInput == 's')
            SetupAudio(true);
        else if (menuInput == 'c')
            ChangeSongSettingsCLI();
        else if (menuInput == 'w')
            SetSeedAndPlay();
        else if (menuInput == 'r')
        {
            RandomConfig();
            SetupAudio(true);
        }
        else if (menuInput == 'p')
            audioSettings.StopAudio();
        else if (menuInput == 'q')
        {
            SDL_Quit();
            exit(0);
        }
#ifdef ENABLE_DEBUG_FUNCTIONS
        else if (menuInput == 'a')
            TestArpeggios();
        else if (menuInput == 'd')
            TestDrums();
        else if (menuInput == 'f')
        {
            SetupAudio();
            Uint8 *tempBuf = new Uint8[songSettings.barLenBytes];
            SlideSquare(440.0f, 880.0f, songSettings.barLenMS, qtrMag, tempBuf, 0);
            SlideSquare(880.0f, 440.0f, songSettings.barLenMS, qtrMag, tempBuf, 0);
        }
        else if (menuInput == 'm')
            PlayMajorScale();
        else if (menuInput == 'n')
            PlayMinorScale();
        else if (menuInput == 'l')
        {
            std::string word = RandomWordFromWordList();
            unsigned int numRep = WordToNumber(word);
            std::cout << "The word is " << word << " and in numeric form that is " << std::to_string(numRep) << "\n";
        }
        else if (menuInput == 't')
            DebugGenerators();
        else if (menuInput == 'k')
            TestGiveScaleKey();
#endif
    }
}

void ChangeSongSettingsCLI()
{
    bool goBack = false;
    
    while (!goBack)
    {
        std::cout << "BPM: " << songSettings.BPM << "\n"
        "Key/Scale: " << songSettings.keyNote << " " << (songSettings.scaleType == ScaleType::Major ? "Major" : "Minor") << "\n"
        "LoFi: " << (songSettings.loFi == true ? "Yep" : "Nup") << "\n\n";
        
        std::cout << "What do you want to change?\n"
            " 1 to change BPM\n"
            " 2 to change key base note\n"
            " 3 to switch key tonality to major / minor\n"
            " 4 to change fidelity\n"
            " 5 to go back\n";
        
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
                    if (!StringIsInt(strInputBPM))
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
                    if (inputBPM > 300 || inputBPM == 0)
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
                if (songSettings.scaleType == ScaleType::Major)
                    songSettings.scaleType = ScaleType::Minor;
                else
                    songSettings.scaleType = ScaleType::Major;
                break;
            }
            case '4':
            {
                if (songSettings.loFi == true)
                {
                    songSettings.loFi = false;
                    audioSettings.audSpecWant.freq = 48000;
                    //audioSettings.audSpecWant.samples = 32768;
                }
                else
                {
                    songSettings.loFi = true;
                    audioSettings.audSpecWant.freq = 8000;
                    //audioSettings.audSpecWant.samples = 4096;
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


bool IsValidSeedInputChar(char ch)
{
    if (int(ch) >= 32 && int(ch) <= 122)
        return true;
    else
        return false;
}

bool ValidSeedInput(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && IsValidSeedInputChar(*it)) ++it;
    return !s.empty() && it == s.end();
}


void SetSeedAndPlay()
{
    std::string stdInputSeed;

    do {
        std::cout << "\nWhat seed word or phrase do you want to use? ";
        std::getline(std::cin, stdInputSeed);
    } while (!ValidSeedInput(stdInputSeed));

    songSettings.rngSeedString = stdInputSeed;

    SeedConfig();

    SetupAudio(true);
}
