del /s /q build_win
mkdir build_win\debug
mkdir build_win\release
cd build_win
cmake --target ALL_BUILD --config Release --build ../


copy ..\SDL2\lib\x64\*.dll debug\

copy ..\SDL2\lib\x64\*.dll release\

copy ..\resources/wordlist.txt debug\

copy ..\resources/wordlist.txt release\


pause