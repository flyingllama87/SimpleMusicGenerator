cmake_minimum_required (VERSION 2.8.13)

project("MusicGen")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Required libraries for linking against nngui (all targets)
set(NNGUI_EXTRA_LIBS "")

# Platform-dependent files for nngui
set(NNGUI_EXTRA_SOURCE "")

# set(SDL_LIBRARY "./SDL2/lib/x64")

include(CheckCXXCompilerFlag)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'RELEASE' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()
string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # Quench annoying deprecation warnings when compiling GLFW on OSX
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-deprecated-declarations")
endif()

if (MSVC)
  # Disable annoying MSVC warnings (all targets)
  add_definitions(/D "_CRT_SECURE_NO_WARNINGS")

  # Parallel build on MSVC (all targets)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

  if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:SSE2")
  endif()
endif()

# Compile with compiler warnings turned on
if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    set(SDL2_MIXER_DIR "SDL2")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4244 /wd4100 /wd4101 /wd4018 /wd4201 /wd4189 /wd4457 /wd4456 /wd4245 /wd4838 /wd4389 /wd4702")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  CHECK_CXX_COMPILER_FLAG("-std=c++14" HAS_CPP14_FLAG)
  CHECK_CXX_COMPILER_FLAG("-std=c++11" HAS_CPP11_FLAG)

  if (HAS_CPP14_FLAG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
  elseif (HAS_CPP11_FLAG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  else()
    message(FATAL_ERROR "Unsupported compiler -- requires C++11 support!")
  endif()
endif()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    message([STATUS] "SETTING SDL2 EMSCRIPTEN FLAGS")
    set(USE_FLAGS "-Wno-narrowing -g -s USE_WEBGL2=1 --profiling -flto -s ALLOW_BLOCKING_ON_MAIN_THREAD=1 -s FULL_ES3=1 -pthread --shell-file shell-minimal.html -s ALLOW_MEMORY_GROWTH=1 -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_MIXER=2 -s USE_SDL_TTF=2")
    #     set(USE_FLAGS "-Wno-narrowing -g4 -s LLD_REPORT_UNDEFINED=0 -s USE_WEBGL2=1 -s FULL_ES3=1 -pthread --shell-file shell-minimal.html -s ALLOW_MEMORY_GROWTH=1 -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_MIXER=2 -s USE_SDL_TTF=2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USE_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${USE_FLAGS}")
    set(CMAKE_EXECUTABLE_SUFFIX .html)
    # add_definitions(-DDEBUG_AUDIO)
else()
    find_package(SDL2 REQUIRED)
    find_package(SDL2_IMAGE REQUIRED)
    find_package(SDL2TTF REQUIRED)
    find_package(SDL2_MIXER REQUIRED)
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)
# include_directories(${CMAKE_SOURCE_DIR}/include ${SDL2_INCLUDE_DIRS})

if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    message([STATUS] "NOT SETTING SDL2 INCLUDES FLAGS")
else()
    message([STATUS] "SETTING SDL2 INCLUDES FLAGS")
    include_directories(${SDL2_INCLUDE_DIR})
    include_directories(${SDL2_IMAGE_INCLUDE_DIR})
    include_directories(${SDL2TTF_INCLUDE_DIR})
    include_directories(${SDL2_MIXER_INCLUDE_DIR})
endif()



# Required core libraries on various platforms
if (WIN32) 
  list(APPEND NNGUI_EXTRA_LIBS opengl32)
  message([STATUS] "In WIN32 PLATFORM check")
elseif (APPLE)
  find_library(cocoa_library Cocoa)
  find_library(opengl_library OpenGL)
  find_library(corevideo_library CoreVideo)
  find_library(iokit_library IOKit)
  list(APPEND NNGUI_EXTRA_LIBS ${cocoa_library} ${opengl_library} ${corevideo_library} ${iokit_library})
  list(APPEND NNGUI_EXTRA_SOURCE darwin.mm)
  message([STATUS] "In OSX PLATFORM check")
elseif(CMAKE_SYSTEM MATCHES "Linux")
  message([STATUS] "In LINUX PLATFORM check")
  list(APPEND NNGUI_EXTRA_LIBS GL Xxf86vm Xrandr Xinerama Xcursor Xi X11 pthread dl rt)
else()
  message([STATUS] "In OTHER PLATFORM check")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/include
                    ${CMAKE_CURRENT_BINARY_DIR})

# message(${NNGUI_EXTRA_LIBS})

set(NNGUI_BASIC_SOURCE "")
list(APPEND NNGUI_BASIC_SOURCE
     sdlgui/button.h
     sdlgui/checkbox.h
     sdlgui/colorpicker.h
     sdlgui/colorwheel.h
     sdlgui/combobox.h
     sdlgui/common.h
     sdlgui/graph.h
     sdlgui/imagepanel.h
     sdlgui/imageview.h
     sdlgui/label.h
     sdlgui/layout.h
     sdlgui/messagedialog.h
     sdlgui/popup.h
     sdlgui/popupbutton.h
     sdlgui/progressbar.h
     sdlgui/screen.h
     sdlgui/slider.h
     sdlgui/stackedwidget.h
     sdlgui/tabheader.h
     sdlgui/tabwidget.h
     sdlgui/textbox.h
     sdlgui/theme.h
     sdlgui/vscrollpanel.h
     sdlgui/switchbox.h
     sdlgui/dropdownbox.h
     sdlgui/widget.h
     sdlgui/window.h
     sdlgui/nanovg.h
     sdlgui/nanovg_rt.h
     mgsrc/MusicGen.h
     mgsrc/reverb.h
     mgsrc/AudioAndMusic.cpp
     mgsrc/Bass.cpp
     mgsrc/Drums.cpp
     mgsrc/Generators.cpp
     mgsrc/Lead.cpp
     mgsrc/UtilFunctions.cpp
     mgsrc/reverb.cpp
     sdlgui/button.cpp
     sdlgui/checkbox.cpp
     sdlgui/colorpicker.cpp
     sdlgui/colorwheel.cpp
     sdlgui/combobox.cpp
     sdlgui/common.cpp
     sdlgui/graph.cpp
     sdlgui/imagepanel.cpp
     sdlgui/imageview.cpp
     sdlgui/label.cpp
     sdlgui/layout.cpp
     sdlgui/loadimages.cpp
     sdlgui/messagedialog.cpp
     sdlgui/resources.cpp
     sdlgui/popup.cpp
     sdlgui/popupbutton.cpp
     sdlgui/progressbar.cpp
     sdlgui/screen.cpp
     sdlgui/slider.cpp
     sdlgui/stackedwidget.cpp
     sdlgui/tabheader.cpp
     sdlgui/tabwidget.cpp
     sdlgui/textbox.cpp
     sdlgui/switchbox.cpp
     sdlgui/dropdownbox.cpp
     sdlgui/theme.cpp
     sdlgui/vscrollpanel.cpp
     sdlgui/widget.cpp
     sdlgui/window.cpp
     sdlgui/nanovg.c
     filesystem.hpp
)
     
option(NANOGUI_BUILD_MUSICGEN "Build Music Generator & WASM interface" ON)

if (NANOGUI_BUILD_MUSICGEN)
  # Build example application if desired
  add_executable(musicGenWASM ${NNGUI_EXTRA_SOURCE} ${NNGUI_BASIC_SOURCE} MusicGenWASM.cpp )
  set_target_properties(
      musicGenWASM PROPERTIES
      VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build/debug")
  target_link_libraries(musicGenWASM ${NNGUI_EXTRA_LIBS} ${SDL2_LIBRARY} ${SDL2IMAGE_LIBRARY} ${SDL2TTF_LIBRARY} ${SDL2_MIXER_LIBRARY})

  # Copy icons for example application
  if (WIN32) 
    file(COPY resources/icons DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/debug)
  else()
    file(COPY resources/icons DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  endif()
endif()
