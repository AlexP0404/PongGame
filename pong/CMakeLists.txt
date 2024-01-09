cmake_minimum_required(VERSION 3.16)

project(pongGame)
set(CMAKE_CXX_STANDARD 17)

if(UNIX)
  add_executable(pong src/ai.cpp src/dot.cpp src/gameLoop.cpp src/main.cpp
                      src/paddle.cpp src/scoreboard.cpp)
elseif(WIN32)
  add_executable(pong WIN32 src/ai.cpp src/dot.cpp src/gameLoop.cpp
                            src/main.cpp src/paddle.cpp src/scoreboard.cpp)
endif()

target_link_libraries(pong engine)

add_custom_command(
  TARGET pong
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/res
          $<TARGET_FILE_DIR:pong>/res
  COMMENT "Creating a symlink to resource directory"
  VERBATIM)
