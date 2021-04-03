# DEMANGLE_SUPPOR
#!/bin/zsh

# em++ -g -std=c++11 -s USE_WEBGL2=1 -s ALLOW_MEMORY_GROWTH=1 -s FULL_ES3=1 -s USE_SDL=2 -s USE_SDL_TTF=2 -o gui_test.html --shell-file shell-minimal.html -s WASM=1 example1.cpp


em++ -I/mnt/c/Users/MJ12/source/nanogui-sdl -I/mnt/c/Users/MJ12/source/nanogui-sdl/include -I/mnt/c/Users/MJ12/source/nanogui-sdl/build -g -std=c++11 -s -s USE_WEBGL2=1 -s FULL_ES3=1 -pthread --shell-file shell-minimal.html -s ALLOW_MEMORY_GROWTH=1 -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_MIXER=2 -s USE_SDL_TTF=2 -o build/example1.html example1.cpp


# emcc:WARNING: invocation: /home/user/src/emsdk/upstream/emscripten/em++.py -std=c++14 -Wno-narrowing -g4 -s LLD_REPORT_UNDEFINED -s USE_WEBGL2=1 -s FULL_ES3=1 -pthread --shell-file shell-minimal.html -s ALLOW_MEMORY_GROWTH=1 -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_MIXER=2 -s USE_SDL_TTF=2 -DNDEBUG -O2 -Wno-narrowing -g4 -s LLD_REPORT_UNDEFINED -s USE_WEBGL2=1 -s FULL_ES3=1 -pthread --shell-file shell-minimal.html -s ALLOW_MEMORY_GROWTH=1 -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_MIXER=2 -s USE_SDL_TTF=2 -O2 @CMakeFiles/example1.dir/objects1.rsp -o example1.html  (in /mnt/c/Users/MJ12/source/nanogui-sdl/build)

# '-I/mnt/c/Users/MJ12/source/nanogui-sdl/include', '-I/mnt/c/Users/MJ12/source/nanogui-sdl', '-I/mnt/c/Users/MJ12/source/nanogui-sdl/build'

# "-Wno-narrowing -g4 -s LLD_REPORT_UNDEFINED 