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



#include <cstddef>
#include <stdexcept>
#include <cstring>
#include <ostream>

#ifndef _MSC_VER
#  if __cplusplus < 201103
#    define CONSTEXPR11_TN
#    define CONSTEXPR14_TN
#    define NOEXCEPT_TN
#  elif __cplusplus < 201402
#    define CONSTEXPR11_TN constexpr
#    define CONSTEXPR14_TN
#    define NOEXCEPT_TN noexcept
#  else
#    define CONSTEXPR11_TN constexpr
#    define CONSTEXPR14_TN constexpr
#    define NOEXCEPT_TN noexcept
#  endif
#else  // _MSC_VER
#  if _MSC_VER < 1900
#    define CONSTEXPR11_TN
#    define CONSTEXPR14_TN
#    define NOEXCEPT_TN
#  elif _MSC_VER < 2000
#    define CONSTEXPR11_TN constexpr
#    define CONSTEXPR14_TN
#    define NOEXCEPT_TN noexcept
#  else
#    define CONSTEXPR11_TN constexpr
#    define CONSTEXPR14_TN constexpr
#    define NOEXCEPT_TN noexcept
#  endif
#endif  // _MSC_VER

class static_string
{
    const char* const p_;
    const std::size_t sz_;

public:
    typedef const char* const_iterator;

    template <std::size_t N>
    CONSTEXPR11_TN static_string(const char(&a)[N]) NOEXCEPT_TN
        : p_(a)
        , sz_(N - 1)
    {}

    CONSTEXPR11_TN static_string(const char* p, std::size_t N) NOEXCEPT_TN
        : p_(p)
        , sz_(N)
    {}

    CONSTEXPR11_TN const char* data() const NOEXCEPT_TN { return p_; }
    CONSTEXPR11_TN std::size_t size() const NOEXCEPT_TN { return sz_; }

    CONSTEXPR11_TN const_iterator begin() const NOEXCEPT_TN { return p_; }
    CONSTEXPR11_TN const_iterator end()   const NOEXCEPT_TN { return p_ + sz_; }

    CONSTEXPR11_TN char operator[](std::size_t n) const
    {
        return n < sz_ ? p_[n] : throw std::out_of_range("static_string");
    }
};

inline
std::ostream&
operator<<(std::ostream& os, static_string const& s)
{
    return os.write(s.data(), s.size());
}

template <class T>
CONSTEXPR14_TN
static_string
type_name()
{
#ifdef __clang__
    static_string p = __PRETTY_FUNCTION__;
    return static_string(p.data() + 31, p.size() - 31 - 1);
#elif defined(__GNUC__)
    static_string p = __PRETTY_FUNCTION__;
#  if __cplusplus < 201402
    return static_string(p.data() + 36, p.size() - 36 - 1);
#  else
    return static_string(p.data() + 46, p.size() - 46 - 1);
#  endif
#elif defined(_MSC_VER)
    static_string p = __FUNCSIG__;
    return static_string(p.data() + 38, p.size() - 38 - 7);
#endif
}

#define EMSCRIPTEN 1


// #define DEBUG_SDL_INIT 0

#define API_URL "http://127.0.0.1:5000/api/"

#if EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/fetch.h>
// #include <emscripten/html5.h>
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

#ifdef EMSCRIPTEN
EM_JS(int, canvas_get_width, (), {
    return yourCanvasElement.width;
    });

EM_JS(int, canvas_get_height, (), {
    return yourCanvasElement.height;
    });
#endif

#undef main

using namespace sdlgui;


class TestWindow : public Screen {
public:

    TestWindow(SDL_Window* pwindow, int rwidth, int rheight)
        : Screen(pwindow, Vector2i(rwidth, rheight), "Music Generator")
    {
        {
            
            getSeedScores();

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
        if (auto* textBox = gfind<TextBox>("seed-string"))
            {
                textBox->setValue(songSettings.rngSeedString);
            }
                        

        Screen::draw(renderer);
    }


    auto& DrawControls()
    {
        
        auto& nwindow = window("Music Generator", Vector2i{ 15, 15 })
            .withLayout<GroupLayout>();

            auto* mainLayout = new GridLayout(
                Orientation::Horizontal,
                1,
                Alignment::Minimum,
                25,
                10
            );

            nwindow.setLayout(mainLayout);

            nwindow.label("Seed Word", "sans-bold");

            auto& seedString = nwindow.textbox(songSettings.rngSeedString);

            cout << "DECLTYPE: " << type_name<decltype(seedString)>() << endl;
            seedString.setEditable(true);
            seedString.setId("seed-string");
            seedString.setAlignment(sdlgui::TextBox::Alignment::Left);
            seedString.setCallback([](const std::string& seedWord) -> bool {
                songSettings.rngSeedString = seedWord;
                std::cout << "seedWord set to " << seedWord;
                return true;
            });

            nwindow.label("Play Controls", "sans-bold");

            nwindow.button("Start", [] {
                if (audioSettings.inited == true) {
                    audioSettings.StopAudio();
                }
                SeedConfig();
                SetupAudio(true);
            }).withTooltip("Start the music");

            nwindow.button("Stop", [] {
                audioSettings.StopAudio();
                SeedConfig();
            });


            // Voting buttons
            nwindow.label("Vote", "sans-bold");

            nwindow.button("Vote Up", [] {
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
            })
                .setBackgroundColor(Color(0, 255, 25, 25));
            nwindow.button("Vote Down", [] {
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
            }).setBackgroundColor(Color(255, 25, 25, 25));

        return seedString;
    }

    void DrawSongList()
    {
        auto& twindow = window("Song List / Score", Vector2i{ 300, 300 })
            .withLayout<GroupLayout>();

        auto* tLayout = new GridLayout(
            Orientation::Horizontal,
            2,
            Alignment::Middle,
            15,
            5
        );

        twindow.setLayout(tLayout);

        // twindow.label("Seed Word", "sans-bold");

        twindow.button(
            "RANDOM", [](bool state) {
                if (state == true)
                {
                    songSettings.rngSeedString = RandomWordFromWordList();
                    // Restart Audio
                    audioSettings.StopAudio();
                    SeedConfig();
                    SetupAudio(true);
                }
            })
            .setBackgroundColor(Color(0, 255, 25, 25));

        twindow.label("∞", "sans-bold");

        for (auto seedScorePair = listOfSeeds.begin(); seedScorePair != listOfSeeds.end(); seedScorePair++)
        {
            // std::string seedStr = *str;
            // std::cout << "seedStr: " << seedStr << "\n";

            std::string* seedStrPtr = new std::string;
            std::string seedStr = *seedStrPtr;
            int score;
            std::tie(seedStr, score) = *seedScorePair;

            auto& seedBtn = twindow.button(seedStr);
            seedBtn.setWidth(200);
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

            // seedBtn.setWidth(200);

            twindow.label(std::to_string(score), "sans-bold");
        }

        performLayout(mSDL_Renderer);
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

// #define EMSCRIPTEN 1

bool quit = false;
SDL_Window* g_window; // Declare a pointer to an SDL_Window
SDL_RendererInfo info;
SDL_RendererInfo renderInfo;
SDL_Renderer* g_renderer;
TestWindow* g_screen;
Fps fps;
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

    g_screen = new TestWindow(g_window, winWidth, winHeight);

#ifdef DEBUG_SDL_INIT
    std::cout << "Complete: Create new window! " << '\n';
    std::cout << "Initiating audio!" << '\n';
#endif

    songSettings.rngSeedString = "covid";

#ifdef DEBUG_SDL_INIT
    std::cout << "About to render!" << '\n';
#endif

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
    SDL_SetWindowSize(g_window, w, h);
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

    // fps.next();
}


#ifdef EMSCRIPTEN

void downloadSucceeded(emscripten_fetch_t* fetch) {
    printf("Finished downloading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);

    // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
    emscripten_fetch_close(fetch); // Free data associated with the fetch.

}

void downloadFailed(emscripten_fetch_t* fetch) {
    printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
    emscripten_fetch_close(fetch); // Also free data on failure.
}

void voteSucceeded(emscripten_fetch_t* fetch) {
    printf("Vote submitted.");

    // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
    emscripten_fetch_close(fetch); // Free data associated with the fetch.

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
    printf("Getting scores succeeded! Downloading %llu bytes from URL %s...\n", fetch->numBytes, fetch->url);

    // Split each line by a comma into separate seed and score values
    std::string input(fetch->data);
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

    // cout << result << endl;

    // Debug print.
    for (auto seedScorePair = listOfSeeds.begin(); seedScorePair != listOfSeeds.end(); seedScorePair++)
    {

        std::string* seedStrPtr = new std::string;
        std::string seedStr = *seedStrPtr;
        int score;
        std::tie(seedStr, score) = *seedScorePair;
        cout << seedStr << " " << score << endl;
    }
    g_screen->DrawControls();
    g_screen->DrawSongList();

    // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
    emscripten_fetch_close(fetch); // Free data associated with the fetch.

}

void getScoresFailed(emscripten_fetch_t* fetch) {
    printf("Getting scores %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
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

    g_screen->DrawControls();
    g_screen->DrawSongList();
}


#endif