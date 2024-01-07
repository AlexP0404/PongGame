cmake_minimum_required(VERSION 3.16)

project(pongGame)
set(CMAKE_CXX_STANDARD 17)

if(UNIX)
  add_executable(
    pong
    src/ai.cpp
    src/dot.cpp
    src/gameLoop.cpp
    src/main.cpp
    src/paddle.cpp
    src/scoreboard.cpp
    res/bounce.wav
    res/lazy.ttf
    res/dot.bmp)
elseif(WIN32)
  add_executable(pong WIN32 src/*.cpp res/bounce.wav res/lazy.ttf res/dot.bmp)
endif()

target_link_libraries(pong engine)
