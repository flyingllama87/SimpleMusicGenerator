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
#include <sdlgui/colorwheel.h>
#include <sdlgui/graph.h>
#include <sdlgui/tabwidget.h>
#include <sdlgui/switchbox.h>
#include <sdlgui/formhelper.h>
#include <memory>
#include <chrono>

#include <mgsrc/MusicGen.h>


#if EMSCRIPTEN
    #include <emscripten.h>
    #include <emscripten/fetch.h>
#endif

#if defined(_WIN32)
    #include <windows.h>
#endif
#include <iostream>

#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#if defined(_WIN32)
#include <SDL_image.h>
#else
#include <SDL2/SDL_image.h>
#endif

using std::cout;
using std::cerr;
using std::endl;

std::vector<std::string> listOfSeeds; 

#undef main

using namespace sdlgui;

class TestWindow : public Screen {
public:

    TestWindow(SDL_Window* pwindow, int rwidth, int rheight)
        : Screen(pwindow, Vector2i(rwidth, rheight), "Music Generator")
    {
        {
            auto& nwindow = window("Music Generator", Vector2i{ 15, 15 })
                .withLayout<GroupLayout>();

            nwindow.label("Seed Word", "sans-bold");

            auto& seedString = nwindow.textbox(songSettings.rngSeedString);
            seedString.setEditable(true);
            seedString.setAlignment(sdlgui::TextBox::Alignment::Left);
            seedString.setCallback([](const std::string& seedWord) -> bool {
                songSettings.rngSeedString = seedWord;
                std::cout << "seedWord set to " << seedWord;
                return true;
            });

            nwindow.label("Play Controls", "sans-bold");

            nwindow.button("Start", [] {
                       SeedConfig();
                       SetupAudio(true);
                   }).withTooltip("Start the music");

            nwindow.button("Stop", [] {
                audioSettings.StopAudio();
                SeedConfig();
            });

            nwindow.label("Settings", "sans-bold");

            auto& loFiToggle = nwindow.button("Lo-Fi Mode");
            loFiToggle.setFlags(Button::ToggleButton);
            loFiToggle.setChangeCallback([](bool state) {
                songSettings.ToggleLofi();
                cout << "Toggle button state: " << state << endl;
                cout << "LoFi state: " << songSettings.loFi << endl;
            });

            auto& nsongList = window("Song List", Vector2i{ 300, 15 })
                .withLayout<GroupLayout>();

            listOfSeeds.push_back("covid");
            listOfSeeds.push_back("partake");
            listOfSeeds.push_back("removal");
            listOfSeeds.push_back("going");
            listOfSeeds.push_back("caboose");
            listOfSeeds.push_back("sebian");
            listOfSeeds.push_back("amuser");
            listOfSeeds.push_back("majesty");
            listOfSeeds.push_back("oops");
            listOfSeeds.push_back("flaxseed");
            listOfSeeds.push_back("reroute");

            nsongList.button(
                "RANDOM", [&seedString](bool state) {
                    if (state == true)
                    {
                        songSettings.rngSeedString = RandomWordFromWordList();
                        seedString.setValue(songSettings.rngSeedString);
                    }
                })
                .setBackgroundColor(Color(0, 255, 25, 25));

            for (auto str = listOfSeeds.begin(); str != listOfSeeds.end(); str++)
            {
                // std::string seedStr = *str;
                // std::cout << "seedStr: " << seedStr << "\n";

                nsongList.button(
                    *str, [&seedString, str](bool state) {
                                songSettings.rngSeedString = *str;
                                seedString.setValue(*str);
                            })
                .setFlags(Button::RadioButton);
            }
        }

        performLayout(mSDL_Renderer);
    }

    ~TestWindow()
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
        if (auto pbar = gfind<ProgressBar>("progressbar")) {
            pbar->setValue(pbar->value() + 0.001f);
            if (pbar->value() >= 1.f)
                pbar->setValue(0.f);
        }

        Screen::draw(renderer);
    }

    virtual void drawContents()
    {
    }

private:
    std::vector<SDL_Texture*> mImagesData;
    int mCurrentImage;
};

class Fps {
public:
    explicit Fps(int tickInterval = 30)
        : m_tickInterval(tickInterval)
        , m_nextTime(SDL_GetTicks() + tickInterval)
    {
    }

    void next()
    {
        SDL_Delay(getTicksToNextFrame());

        m_nextTime += m_tickInterval;
    }

private:
    const int m_tickInterval;
    Uint32 m_nextTime;

    Uint32 getTicksToNextFrame() const
    {
        Uint32 now = SDL_GetTicks();

        return (m_nextTime <= now) ? 0 : m_nextTime - now;
    }
};

void MainLoop();

bool quit = false;
SDL_Window* g_window; // Declare a pointer to an SDL_Window
SDL_RendererInfo info;
SDL_RendererInfo renderInfo;
SDL_Renderer* g_renderer;
TestWindow* g_screen;
Fps fps;
SDL_Event e;

// #define EMSCRIPTEN 1

#ifdef EMSCRIPTEN

    void downloadSucceeded(emscripten_fetch_t *fetch) {
        printf("Finished downloading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);
        // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
        emscripten_fetch_close(fetch); // Free data associated with the fetch.
    }

    void downloadFailed(emscripten_fetch_t *fetch) {
        printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
        emscripten_fetch_close(fetch); // Also free data on failure.
    }

#endif

int main(int /* argc */, char** /* argv */)
{
    char rendername[256] = { 0 };

    std::cout << "Complete: Renderer info. Enabling audio:" << '\n';

    if (SDL_Init(SDL_INIT_AUDIO) < 0)
        std::cout << "Could not initialise SDL";
    else
        std::cout << "SDL initialised!! Seeding random number gen\n";

    // Seed random number gen in main thread
    srand(std::chrono::system_clock::now().time_since_epoch().count());

    SDL_Init(SDL_INIT_VIDEO); // Initialize SDL2
    std::cout << "Complete: init video" << '\n';

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

    int winWidth = 1024;
    int winHeight = 768;

    std::cout << "Complete: windows settings " << '\n';
    // Create an application window with the following settings:
    g_window = SDL_CreateWindow(
        "An SDL2 window", //    const char* title
        SDL_WINDOWPOS_UNDEFINED, //    int x: initial x position
        SDL_WINDOWPOS_UNDEFINED, //    int y: initial y position
        winWidth, //    int w: width, in pixels
        winHeight, //    int h: height, in pixels
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE //    Uint32 flags: window options, see docs
        );
    SDL_SetWindowSize(g_window, winWidth, winHeight);

    std::cout << "Complete: Create window: " << '\n';

    // Check that the window was successfully made
    if (g_window == NULL) {
        // In the event that the window could not be made...
        std::cout << "Could not create window: " << SDL_GetError() << '\n';
        SDL_Quit();
        return 1;
    }

    std::cout << "PASSED: error on windows creation " << '\n';
    std::cout << "Errors thus far: " << SDL_GetError() << '\n';

    auto context = SDL_GL_CreateContext(g_window);

    std::cout << "Complete: create gl context " << '\n';
    std::cout << "Errors thus far: " << SDL_GetError() << '\n';

    for (int it = 0; it < SDL_GetNumRenderDrivers(); it++) {
        SDL_GetRenderDriverInfo(it, &info);
        strcat(rendername, info.name);
        strcat(rendername, " ");
        std::cout << "RENDERER NAME: " << rendername << '\n';
    }

    std::cout << "Complete: acquire renderer name " << '\n';
    std::cout << "Errors thus far: " << SDL_GetError() << '\n';

#ifdef WIN32
    g_renderer = SDL_CreateRenderer(g_window, 2, SDL_RENDERER_ACCELERATED);
#else
    g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
#endif

    std::cout << "Errors thus far: " << SDL_GetError() << '\n';

    SDL_SetRenderDrawBlendMode(g_renderer, SDL_BLENDMODE_BLEND);

    std::cout << "Complete: Created render page with hardware accel" << '\n';

    SDL_GetRendererInfo(g_renderer, &renderInfo);
    std::cout << "Selected Renderer name: " << renderInfo.name << '\n';

    std::cout << "Errors thus far: " << SDL_GetError() << '\n';

    std::cout << "VALS: " << winWidth << " " << winHeight << '\n';

    g_screen = new TestWindow(g_window, winWidth, winHeight);

    std::cout << "Complete: Create new window! " << '\n';

    std::cout << "Initiating audio!" << '\n';

    songSettings.rngSeedString = "covid";

    std::cout << "About to render!" << '\n';

#ifdef EMSCRIPTEN
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "GET");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.onsuccess = downloadSucceeded;
    attr.onerror = downloadFailed;
    emscripten_fetch(&attr, "http://morganrobertson.net/wp-content/uploads/3_cropped_130x130.png");
    emscripten_set_main_loop(MainLoop, 30, 1);
#else
    while (quit == false) {
        MainLoop();
    }
#endif

    return 0;
}

void MainLoop()
{
    //Handle events on queue
    while (SDL_PollEvent(&e) != 0) {
        //User requests quit
        if (e.type == SDL_QUIT) {
            quit = true;
        }
        g_screen->onEvent(e);
    }

    SDL_SetRenderDrawColor(g_renderer, 0xd3, 0xd3, 0xd3, 0xff);
    SDL_RenderClear(g_renderer);

    g_screen->drawAll();

    // Render the rect to the screen
    SDL_RenderPresent(g_renderer);

    // fps.next();
}