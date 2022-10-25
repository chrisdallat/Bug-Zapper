#!/bin/bash
export CC=/usr/bin/clang
export CXX=/usr/bin/clang++

mkdir -p build

if [ "$1" == "--gen" ]; then
    cd build; cmake ../; make; cd .. 

elif [ "$1" == "--make" ]; then
    cd build; make; cd .. 

elif [ "$1" == "--run" ]; then

    if [ "$#" -eq 1 ]; then echo "You must provide an binary name to run."
    else ./build.sh --clean-all; ./build.sh --"$2"; cd build; cmake ../; make "$2" -j$('nproc') ; ./"$2" cd .. 
fi

elif [ "$1" == "--clean" ]; then
    cd build; make clean; cd .. 

elif [ "$1" == "--clean-all" ]; then
    rm -rf build

elif [ "$1" == "-h"  ] || [ "$1" == "--help" ];  then
   echo "--gen:                 Generate make files with CMakeLists.txt and make."
   echo "--make:                Run make in 'build' to build binaries."
   echo "--run <binary_name>:   Runs the respective <binary_name> in 'build'."

else
  echo "option" "$1" "is invalid."
fi


