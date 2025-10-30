/*
Morgan Robertson 2021
*/

#include <sdlgui/screen.h>
#include <sdlgui/window.h>
#include <sdlgui/layout.h>
#include <sdlgui/label.h>
#include <sdlgui/checkbox.h>
#include <sdlgui/button.h>
#include <sdlgui/toolbutton.h>
#include <sdlgui/popupbutton.h>
#include <sdlgui/combobox.h>
#include <sdlgui/dropdownbox.h>
#include <sdlgui/progressbar.h>
#include <sdlgui/entypo.h>
#include <sdlgui/messagedialog.h>
#include <sdlgui/textbox.h>
#include <sdlgui/slider.h>
#include <sdlgui/imagepanel.h>
#include <sdlgui/imageview.h>
#include <sdlgui/vscrollpanel.h>
#include <sdlgui/vswrap.h>
#include <sdlgui/colorwheel.h>
#include <sdlgui/graph.h>
#include <sdlgui/tabwidget.h>
#include <sdlgui/switchbox.h>
#include <sdlgui/formhelper.h>
#include <memory>
#include <chrono>

#include <mgsrc/MusicGen.h>


#define EMSCRIPTEN 1

// #define DEBUG_SDL_INIT 0

// #define API_URL "http://127.0.0.1:5000/api/"
#define API_URL "https://musicgen.net/api/"


#if EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/fetch.h>
#include <emscripten/html5.h>
#endif

#if defined(_WIN32)
#include <windows.h>
#endif
#include <iostream>

#if defined(_WIN32)
#include <SDL.h>
#include <SDL_image.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif

using std::cout;
using std::cerr;
using std::endl;

float g_vspPos = 0.0;

std::vector<std::tuple<std::string, int>> listOfSeeds;

#ifdef EMSCRIPTEN
void voteSucceeded(emscripten_fetch_t* fetch);
void voteFailed(emscripten_fetch_t* fetch);
void downloadFailed(emscripten_fetch_t* fetch);
void downloadSucceeded(emscripten_fetch_t* fetch);
void getSeedScores();
void getScoresSuccess(emscripten_fetch_t* fetch);
void getScoresFailed(emscripten_fetch_t* fetch);
#endif

#undef main

using namespace sdlgui;


class MainWindow : public Screen {
public:

    MainWindow(SDL_Window* pwindow, int rwidth, int rheight)
        : Screen(pwindow, Vector2i(rwidth, rheight), "Music Generator")
    {
        {
            getSeedScores();
        }

        performLayout(mSDL_Renderer);
    }

    ~MainWindow()
    {
    }

    virtual bool keyboardEvent(int key, int scancode, int action, int modifiers)
    {
        if (Screen::keyboardEvent(key, scancode, action, modifiers))
            return true;
        return false;
    }

    virtual void draw(SDL_Renderer* renderer)
    {
        if (auto* textBox = gfind<TextBox>("seed-string"))
        {
            textBox->setValue(songSettings.rngSeedString);
        }

        if (auto* vsp = gfind<VScrollPanel>("vsp"))
        {
            g_vspPos = vsp->getScrollPos();
        }

        Screen::draw(renderer);
    }


    void DrawControls()
    {   

        if (auto* twindow = gfind<Window>("controls"))
        {
            // Skip drawing controls if they already exist.
            return;
        }

        // Calculate where to draw the controls window
        int WinWidth, WinHeight, xPos;
        emscripten_get_canvas_element_size("#canvas", &WinWidth, &WinHeight);
        xPos = WinWidth / 2 - (925 / 2);
        std::cout << xPos << "\n";

        auto& nwindow = window("Music Generator", Vector2i{ xPos, 15 });

            auto* mainLayout = new GridLayout(
                Orientation::Horizontal,
                1,
                Alignment::Fill,
                15,
                10
            );

            nwindow.setLayout(mainLayout);

            nwindow.setFixedWidth(310);

            nwindow.label("Seed Word", "sans-bold");

            nwindow.setId("controls");

            auto& seedString = nwindow.textbox(songSettings.rngSeedString);

            seedString.setEditable(true);
            seedString.setId("seed-string");
            seedString.setAlignment(sdlgui::TextBox::Alignment::Left);
            seedString.setFormat("^[a-zA-Z0-9 _]{1,15}$");
            seedString.setFixedHeight(50);

            seedString.setCallback([](const std::string& seedWord) -> bool {
                songSettings.rngSeedString = seedWord;
                std::cout << "seedWord set to " << seedWord;
                return true;
            });

            nwindow.label("Controls", "sans-bold");

            nwindow.button("Play", [] {
                if (audioSettings.inited == true) {
                    audioSettings.StopAudio();
                }
                SeedConfig();
                SetupAudio(true);
            }).setFixedHeight(50);

            nwindow.button("Stop", [] {
                audioSettings.StopAudio();
                SeedConfig();
            }).setFixedHeight(50);


            // Voting buttons
            nwindow.label("Vote", "sans-bold");

            auto& voteUpBtn = nwindow.button("Vote Up", [] {
                // Send HTTP Req
                emscripten_fetch_attr_t attr;
                emscripten_fetch_attr_init(&attr);
                strcpy(attr.requestMethod, "POST");
                const char* headers[] = { "Content-Type", "application/x-www-form-urlencoded; charset=utf-8", 0 };
                attr.requestHeaders = headers;
                attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
                attr.onsuccess = voteSucceeded;
                attr.onerror = voteFailed;
                std::string apiEndpoint = API_URL;
                apiEndpoint += "UpVote";

                // Why do I have to do all this?
                std::string strHttpPostData("seed=", 5);
                strHttpPostData = strHttpPostData.append(songSettings.rngSeedString.c_str());
                // std::cout << strHttpPostData << endl;
                char* httpPostData = (char*)malloc(128);
                memset(httpPostData, '\0', sizeof(128));
                strcpy(httpPostData, strHttpPostData.c_str());
                const char* seedName = httpPostData;
                attr.requestData = httpPostData;
                attr.requestDataSize = strlen(attr.requestData);
                // Send the request
                emscripten_fetch(&attr, apiEndpoint.c_str());
            });
            voteUpBtn.setBackgroundColor(Color(60, 85, 63, 255));
            voteUpBtn.setFixedHeight(50);

            auto& voteDownBtn = nwindow.button("Vote Down", [] {
                // Send HTTP Req
                emscripten_fetch_attr_t attr;
                emscripten_fetch_attr_init(&attr);
                strcpy(attr.requestMethod, "POST");
                const char* headers[] = { "Content-Type", "application/x-www-form-urlencoded; charset=utf-8", 0 };
                attr.requestHeaders = headers;
                attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
                attr.onsuccess = voteSucceeded;
                attr.onerror = voteFailed;
                std::string apiEndpoint = API_URL;
                apiEndpoint += "DownVote";

                // Why do I have to do all this?
                std::string strHttpPostData("seed=", 5);
                strHttpPostData = strHttpPostData.append(songSettings.rngSeedString.c_str());
                // std::cout << strHttpPostData << endl;
                char* httpPostData = (char*)malloc(128);
                memset(httpPostData, '\0', sizeof(128));
                strcpy(httpPostData, strHttpPostData.c_str());
                const char* seedName = httpPostData;
                attr.requestData = httpPostData;
                attr.requestDataSize = strlen(attr.requestData);
                // Send the request
                emscripten_fetch(&attr, apiEndpoint.c_str());
            });
            voteDownBtn.setBackgroundColor(Color(85, 63, 63, 255));
            voteDownBtn.setFixedHeight(50);

        return;
    }

    void DestroySongList()
    {
        if (auto* twindow = gfind<Window>("twindow"))
        {
            this->disposeWindow(twindow);
        }
    }

    void DrawSongList()
    {

        // Calculate where to draw the Song List window
        int WinWidth, WinHeight, xPos;
        emscripten_get_canvas_element_size("#canvas", &WinWidth, &WinHeight);
        xPos = WinWidth / 2 - (925 / 2) + 350;

        auto& twindow = window("Song List & Scores", Vector2i{ xPos, 15 })
            .withLayout<GroupLayout>();

        twindow.setId("twindow");

        int width = 525;
        int height = WinHeight - 100;

        // attach a vertical scroll panel
        auto vscroll = new VScrollPanel(&twindow);
        vscroll->setFixedSize({width, height});
        // vscroll->setHeight(height);
        vscroll->setId("vsp");


        // vscroll should only have *ONE* child. this is what `wrapper` is for
        auto wrapper = new VSWrapper(vscroll);
        wrapper->setId("vsp-wrapper");
       
        auto* tLayout = new GridLayout(
            Orientation::Horizontal,
            2,
            Alignment::Fill,
            5,
            10
        );

        wrapper->setLayout(tLayout);

        auto& randomBtn = wrapper->button(
            "RANDOM", [](bool state) {
                if (state == true)
                {
                    songSettings.rngSeedString = RandomWordFromWordList();
                    // Restart Audio
                    audioSettings.StopAudio();
                    SeedConfig();
                    SetupAudio(true);
                }
            });
            randomBtn.setBackgroundColor(Color(60, 85, 63, 255));
            randomBtn.setFixedHeight(50);

        wrapper->label("  ∞", "sans-bold");

        for (auto seedScorePair = listOfSeeds.begin(); seedScorePair != listOfSeeds.end(); seedScorePair++)
        {
            // std::string seedStr = *str;
            // std::cout << "seedStr: " << seedStr << "\n";

            std::string* seedStrPtr = new std::string;
            std::string seedStr = *seedStrPtr;
            int score;
            std::tie(seedStr, score) = *seedScorePair;

            auto& seedBtn = wrapper->button(seedStr);
            // rseedBtn.setWidth(200);
            seedBtn.setFlags(Button::RadioButton);
            seedBtn.setCallback([seedScorePair]() {
                // Split Seed name + score tuple
                int score;
                std::string seedStr;
                std::tie(seedStr, score) = *seedScorePair;
                // Set seed value
                songSettings.rngSeedString = seedStr;
                // Restart Audio
                audioSettings.StopAudio();
                SeedConfig();
                SetupAudio(true);
            });
            // seedBtn.setVisible(false);

            seedBtn.setFixedHeight(50);

            auto& scoreLabel = wrapper->label("  " + std::to_string(score), "sans-bold");
        }

        // std::cout << "g_vspPos = " << g_vspPos << "\n";
        vscroll->scrollTo(g_vspPos);

        performLayout(mSDL_Renderer);

    }

    virtual void drawContents()
    {
    }

private:
    std::vector<SDL_Texture*> mImagesData;
    int mCurrentImage;
};

void MainLoop();

bool quit = false;
SDL_Window* g_window; // Declare a pointer to an SDL_Window
SDL_RendererInfo info;
SDL_RendererInfo renderInfo;
SDL_Renderer* g_renderer;
MainWindow* g_screen;
SDL_Event e;


int main(int /* argc */, char** /* argv */)
{

    char rendername[256] = { 0 };

#ifdef DEBUG_SDL_INIT
    std::cout << "Complete: Renderer info. Enabling audio:" << '\n';
#endif

    if (SDL_Init(SDL_INIT_AUDIO) < 0)
        std::cout << "Could not initialise SDL";
    else
        std::cout << "SDL initialised!! Seeding random number gen\n";

    // Seed random number gen in main thread
    srand(std::chrono::system_clock::now().time_since_epoch().count());

    SDL_Init(SDL_INIT_VIDEO); // Initialize SDL2

#ifdef DEBUG_SDL_INIT
    std::cout << "Complete: init video" << '\n';
#endif

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

    int winWidth = 1024;
    int winHeight = 768;

#ifdef DEBUG_SDL_INIT
    std::cout << "Complete: windows settings " << '\n';
#endif


    emscripten_get_canvas_element_size("#canvas", &winWidth, &winHeight);
    
    // Create an application window with the following settings:
    g_window = SDL_CreateWindow(
        "An SDL2 window", //    const char* title
        SDL_WINDOWPOS_UNDEFINED, //    int x: initial x position
        SDL_WINDOWPOS_UNDEFINED, //    int y: initial y position
        winWidth, //    int w: width, in pixels
        winHeight, //    int h: height, in pixels
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE //    Uint32 flags: window options, see docs
    );

#ifdef DEBUG_SDL_INIT
    std::cout << "Complete: Create window: " << '\n';
#endif

    // Check that the window was successfully made
    if (g_window == NULL) {
        // In the event that the window could not be made...
        std::cout << "Could not create window: " << SDL_GetError() << '\n';
        SDL_Quit();
        return 1;
    }

#ifdef DEBUG_SDL_INIT
    std::cout << "PASSED: error on windows creation " << '\n';
    std::cout << "Errors thus far: " << SDL_GetError() << '\n';
#endif


    auto context = SDL_GL_CreateContext(g_window);


#ifdef DEBUG_SDL_INIT
    std::cout << "Complete: create gl context " << '\n';
    std::cout << "Errors thus far: " << SDL_GetError() << '\n';
#endif

    for (int it = 0; it < SDL_GetNumRenderDrivers(); it++) {
        SDL_GetRenderDriverInfo(it, &info);
        strcat(rendername, info.name);
        strcat(rendername, " ");
#ifdef DEBUG_SDL_INIT
        std::cout << "RENDERER NAME: " << rendername << '\n';
#endif
    }

#ifdef DEBUG_SDL_INIT
    std::cout << "Complete: acquire renderer name " << '\n';
    std::cout << "Errors thus far: " << SDL_GetError() << '\n';
#endif

#ifdef WIN32
    g_renderer = SDL_CreateRenderer(g_window, 2, SDL_RENDERER_ACCELERATED);
#else
    g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
#endif

#ifdef DEBUG_SDL_INIT
    std::cout << "Errors thus far: " << SDL_GetError() << '\n';
#endif

    SDL_SetRenderDrawBlendMode(g_renderer, SDL_BLENDMODE_BLEND);

#ifdef DEBUG_SDL_INIT
    std::cout << "Complete: Created render page with hardware accel" << '\n';
#endif

    SDL_GetRendererInfo(g_renderer, &renderInfo);

#ifdef DEBUG_SDL_INIT
    std::cout << "Selected Renderer name: " << renderInfo.name << '\n';
    std::cout << "Errors thus far: " << SDL_GetError() << '\n';
    std::cout << "VALS: " << winWidth << " " << winHeight << '\n';
#endif

    g_screen = new MainWindow(g_window, winWidth, winHeight);
    g_screen->setId("MainWindow");

#ifdef DEBUG_SDL_INIT
    std::cout << "Complete: Create new window! " << '\n';
    std::cout << "Initiating audio!" << '\n';
#endif

    songSettings.rngSeedString = "covid";

#ifdef DEBUG_SDL_INIT
    std::cout << "About to render!" << '\n';
#endif

    // Test 3: Check that resizing the canvas works as well.
    // emscripten_set_canvas_element_size("#canvas", 1024, 768);
    // emscripten_get_canvas_element_size("#canvas", &w, &h);
    // printf("w:%d,h:%d\n", w,h);


#ifdef EMSCRIPTEN
    emscripten_set_main_loop(MainLoop, 30, 1);
#else
    while (quit == false) {
        MainLoop();
    }
#endif

    return 0;
}

void update_screen_size(int w, int h)
{
    int w2, h2;
    emscripten_set_canvas_element_size("#canvas", w, h);
    SDL_SetWindowSize(g_window, w, h);

    emscripten_get_canvas_element_size("#canvas", &w2, &h2);
}

void MainLoop()
{
    //Handle events on queue
    while (SDL_PollEvent(&e) != 0) {
        //User requests quit
        if (e.type == SDL_QUIT) {
            quit = true;
        }

        if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED)
        {
            update_screen_size(e.window.data1, e.window.data2);
        }

        g_screen->onEvent(e);
    }

    SDL_SetRenderDrawColor(g_renderer, 0xd3, 0xd3, 0xd3, 0xff);
    SDL_RenderClear(g_renderer);

    g_screen->drawAll();

    // Render the rect to the screen
    SDL_RenderPresent(g_renderer);

}


#ifdef EMSCRIPTEN

void downloadSucceeded(emscripten_fetch_t* fetch) {
    printf("Finished downloading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);

    emscripten_fetch_close(fetch); // Free data associated with the fetch.

}

void downloadFailed(emscripten_fetch_t* fetch) {
    printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
    emscripten_fetch_close(fetch); // Also free data on failure.
}

void voteSucceeded(emscripten_fetch_t* fetch) {
    printf("\nVote submitted! ");

    emscripten_fetch_close(fetch); // Free data associated with the fetch.

    g_screen->DestroySongList();
    getSeedScores();
}

void voteFailed(emscripten_fetch_t* fetch) {
    printf("Vote failed.  HTTP endpoint: %s , HTTP failure status code: %d.\n", fetch->url, fetch->status);
    emscripten_fetch_close(fetch); // Also free data on failure.
}


void getSeedScores()
{
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "GET");
    // attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_SYNCHRONOUS;
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.onsuccess = getScoresSuccess;
    attr.onerror = getScoresFailed;
    std::string apiEndpoint = API_URL;
    apiEndpoint += "GetScores";
    // Send the request, should block here
    emscripten_fetch(&attr, apiEndpoint.c_str());
}


void getScoresSuccess(emscripten_fetch_t* fetch) {
    
    
    printf("Succeeded getting scores! Downloaded %llu bytes from URL %s...\n", fetch->numBytes, fetch->url);

    // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];

    listOfSeeds.clear();
    
    // Split each line by a comma into separate seed and score values
    std::string input(fetch->data, fetch->numBytes-1);
    std::string result;
    std::istringstream iss(input);

    for (std::string line; std::getline(iss, line); )
    {
        result += line + "\n";

        std::string delimiter = ",";
        int commaPos = line.find(delimiter);

        if (commaPos > 0)
        {
            std::string seed = line.substr(0, line.find(delimiter));
            // cout << seed << " ";
            std::string score = line.substr(line.find(delimiter) + 1, line.length() - 1);
            // cout << score << endl;
            int intScore = stoi(score);
            listOfSeeds.push_back(std::tuple<std::string, int>{seed, intScore});
        }

    }

    /*
    // Debug print.
    for (auto seedScorePair = listOfSeeds.begin(); seedScorePair != listOfSeeds.end(); seedScorePair++)
    {
        std::string* seedStrPtr = new std::string;
        std::string seedStr = *seedStrPtr;
        int score;
        std::tie(seedStr, score) = *seedScorePair;
        // cout << seedStr << " " << score << endl;
    }

    */
   
    g_screen->DrawControls();
    g_screen->DrawSongList();

    emscripten_fetch_close(fetch); // Free data associated with the fetch.

}

void getScoresFailed(emscripten_fetch_t* fetch) {
    printf("Getting scores %s FAILED!, HTTP failure status code: %d.\n", fetch->url, fetch->status);
    emscripten_fetch_close(fetch); // Also free data on failure.

    listOfSeeds.push_back(std::tuple<std::string, int>{"reroute", 10});
    listOfSeeds.push_back(std::tuple<std::string, int>{"going", 10});
    listOfSeeds.push_back(std::tuple<std::string, int>{"covid", 8});
    listOfSeeds.push_back(std::tuple<std::string, int>{"caboose", 6});
    listOfSeeds.push_back(std::tuple<std::string, int>{"partake", 2});
    listOfSeeds.push_back(std::tuple<std::string, int>{"removal", 2});
    listOfSeeds.push_back(std::tuple<std::string, int>{"flaxseed", 2});
    listOfSeeds.push_back(std::tuple<std::string, int>{"serbian", 2});
    listOfSeeds.push_back(std::tuple<std::string, int>{"amuser", 2});
    listOfSeeds.push_back(std::tuple<std::string, int>{"majesty", 2});
    listOfSeeds.push_back(std::tuple<std::string, int>{"oops", 2});
    listOfSeeds.push_back(std::tuple<std::string, int>{"MMMMMMMMMMMMMMM", -1});


    g_screen->DrawControls();
    g_screen->DrawSongList();
}


#endif