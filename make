#!/bin/bash
rm -rfv build
mkdir build

build_os() {
  case "$1" in
    win32)
        CC_PREFIX="i686-w64-mingw32-"
        SO_EXTENSION=".dll"
        EXE_EXTENSION=".exe"
        CFLAGS="-static-libgcc"
        ;;
    linux)
        ;;
    custom)
      ;;
    *)
      echo "invalid target platform"
      exit 1
  esac

  if [ "$SO_EXTENSION" = "" ]; then
    SO_EXTENSION=".so"
  fi
  if [ "$EXE_EXTENSION" = "" ]; then
    EXE_EXTENSION=""
  fi

  mkdir build/$1
  mkdir build/$1/obj
  for file in $(find src/nedclib | grep "\.cpp$"); do
    ${CC_PREFIX}g++ $CFLAGS -flto -DNEDCLIB_EXPORTS -c -g -fPIC -o build/$1/obj/$(basename $file).o -Wall -O2 -std=c++0x -I src/nedclib -I src/nedclib/rawbmp $file
  done
  ar rcs build/$1/nedclib.a build/$1/obj/*
  ${CC_PREFIX}g++ $LDFLAGS -flto -fPIC -o build/$1/nedclib${SO_EXTENSION} -shared -Wall -O2 -std=c++0x build/$1/obj/*

  for prog in nedcenc raw2bmp nedcmake nevpk; do
    ${CC_PREFIX}g++ $CFLAGS -flto -O2 -Wall -o build/$1/${prog}${EXE_EXTENSION} -I src/nedclib src/$prog.cpp build/$1/nedclib.a
  done
}

if [ ! "$1" = "" ]; then
  build_os $1
else
  build_os linux
  build_os win32
fi
