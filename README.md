### Description:

Simple procedural music generator program.  Tested on iOS, windows & macOS.  Uses SDL for audio hardware abstraction.  Everything is my own except the modified reverb functions.

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
- CLI interface
- basic GUI interface via iOS

Unimplemented:
- Accidentals in some areas!
- SDL2 GUI & WebAssembly GUI
- Updated iOS GUI
- Vibrato / LFO?
- Backend seed voting server

What I've learnt:
- Coming but my own notes:
- Building up small, pure functions DOES make it easier to restructure your program
- Memory management
- Endianness & off by one issues. Issues with endianness caused a bug that took me a long, long time to track down
- More modern-ish c++ stdlib
- Building a static lib and including that in other projects
- Some swift/iOS stuff
- Some music theory and audio programming skillz