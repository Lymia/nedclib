#!/bin/bash
rm -rfv build
mkdir build

mkdir build/obj
for file in $(find nedclib | grep "\.cpp$"); do
  g++ -flto -c -g -fPIC -o build/obj/$(basename $file).o -Wall -O2 -std=c++0x -I nedclib -I nedclib/rawbmp $file
done
ar rcs build/nedclib.a build/obj/*
g++ -flto -fPIC -o build/nedclib.so -shared -Wall -O2 -std=c++0x build/obj/*

g++ -flto -O2 -Wall -o build/nedcenc nedcenc/*.cpp build/nedclib.a
g++ -flto -O2 -Wall -o build/raw2bmp raw2bmp/*.cpp build/nedclib.a
g++ -flto -O2 -Wall -o build/nedcmake nedcmake/*.cpp build/nedclib.a
g++ -flto -O2 -Wall -o build/nevpk nevpk/*.cpp build/nedclib.a
