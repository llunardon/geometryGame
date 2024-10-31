# Geometry Game

![screenshot](screenshot.png)

### NOTES:

- Developed using CMake version > 3.30 on Windows
- Compiler used: mingw64
- SFML version 2.6.1

### Build
```
mkdir external

git clone --branch 2.6.1 https://github.com/SFML/SFML.git external\SFML

OR

git clone --branch 2.6.1 git@github.com:SFML/SFML.git external/SFML

cmake -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH=[gcc.exe bin path] -DCMAKE_CXX_COMPILER:FILEPATH=[g++.exe bin path] --no-warn-unused-cli -S[project dir path] -B[build dir path] -G "MinGW Makefiles"

cmake --build [build dir name] --config Debug --target all -j 10 --
```

Then run the executable in the build directory
