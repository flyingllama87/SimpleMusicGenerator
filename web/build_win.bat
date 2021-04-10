del /s /q build
mkdir build
cd build
cmake --target ALL_BUILD --config Release --build ../
pause
copy ../SDL2/lib/x64/*.dll build/debug/