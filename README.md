### Description:

Simple music generator program using SDL.  Tested on iOS, windows & macOS.

Implemented:
- Wave generators
- Noise generation
- Fadein/Fadeout/slide effects
- In-key frequency generation
- Music callback function
- Drum sound generation
- Drum pattern/track generation
- Debug functions (Testing scale generation, test sound generators, dumping buffers to disk etc.)
- Bass track generation
- Lead track generation
- Arpeggio generation
- CLI interface options such as BPM etc.
- basic GUI interface via iOS

Unimplemented:
- Accidentals in some areas!!
- SDL2 GUI
- WebAssembly support
- Updated iOS GUI
- Vibrato
- More patterns! 'fligagoos', alternate between, etc.

What I've learnt:
- Coming but my own notes:
- Building up small, pure functions DOES make it easier to restructure your program
- Little Endian/Big Endian & off by one issues.  A bug that took me a long, long time to track down.
- More modern-ish c++ stdlib.
- Building a static lib and including that in other projects.
- Some swift/iOS stuff.
