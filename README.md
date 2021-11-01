# Morgan's music generator.

### Description:

Generates and plays procedureal chip-inspired music.

Runs on the web at https://musicgen.net.

Runs on windows & macOS with a CLI.

Runs on iOS with a custom graphical client (out of date).

### How to use

Go to https://musicgen.net.

Select a seed from the score list to the right or type in new a "seed word" and click "Play" to hear never ending chip-inspired music. You can vote with the voting buttons and the result will be saved to the server.

It doesn't work in Safari or iOS platforms unfortunately (Blame Apple) but will work with any modern browser including android.

### Tech

Music generation and frontends written in c++ with the exception of iOS. Targets the web with WASM/emscripten. Backend voting server in Python/Flask/SQLite. Deployed on AWS Elastic Beanstalk.

Frameworks used/credits:

- SDL2 used for hardware abstraction. 
- Modified version of Dale Rank's nanogui-sdl UI library used for WASM client https://github.com/dalerank/nanogui-sdl
- Modified version of Sergei Gorskiy's IFMO_Reverb https://github.com/SergeiGorskiy/IFMO_Reverb 

Everything else is my own.

Runs fairly lightweight.

### Music

- 3 audio tracks - Drums, Bass and Lead
- 3 lead instruments types
- Frequencies generated in correct musical key. All notes & major/minor scales supported
- BPM from 60 to 260
- All sound is generated programmatically using waves, noise and effects
- All music in 4/4

### Implemented:
- Sine, Square & SawTooth Wave Generators.
- Noise generation
- Fadein/Fadeout/slide/reverb effects
- Drum sound generation - hi-hat, snare and kick drum
- Drum pattern/track generation
- Debug functions (Testing scale generation, test sound generators, dumping buffers to disk etc.)
- Bass track generation
- Lead track generation
- Arpeggio generation
- CLI interface
- Backend python/flask seed voting server
- SDL2 GUI & WebAssembly GUI
- basic GUI interface via iOS (currently broken)

### What I've learnt:
- More modern C++ 
- Clang & CMake
- ASync Threading, buffer & memory management
- Debugging tools incl. Chrome
- Building & implementing a c++ static lib for iOS
- Music theory and audio programming
- UI programming