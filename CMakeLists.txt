cmake_minimum_required(VERSION 3.12)
project(reading)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/cmake-build-debug)
SET(CMAKE_CXX_FLAGS "-Ofast -mtune=native -march=native -flto -ffat-lto-objects -fuse-linker-plugin")
add_executable(Engine Union.h Prng.h BitPattern.h Position.h main.cpp Board.h Node.h types.h Search.h  Util/Util.cpp Util/Util.h)
add_executable(testing testing.cpp types.h)

