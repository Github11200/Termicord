#!/bin/bash

port=8000
while [[ $# -gt 0 ]]; do 
  if [[ "$1" == "--port" ]]; then
    port="$2" 
    break 
  fi 
  break 
done

cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/home/user/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build --target client

./build/client/client $port 2>&1
