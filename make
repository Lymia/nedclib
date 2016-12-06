#!/bin/bash
rm -rfv build
mkdir build

g++ -g -fPIC -o build/nedclib.so -shared -Wall -std=c++0x -I nedclib -I nedclib/rawbmp/ `find nedclib | grep "\.cpp$"`
g++ -o build/nedcenc nedcenc/*.cpp build/nedclib.so
g++ -o build/raw2bmp raw2bmp/*.cpp build/nedclib.so
g++ -o build/nedcmake nedcmake/*.cpp build/nedclib.so
g++ -o build/nevpk nevpk/*.cpp build/nedclib.so
