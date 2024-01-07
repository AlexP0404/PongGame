cmake_minimum_required(VERSION 3.16)
project(pongGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

option(ENGINE_USE_VLK "Use the vulkan & GLFW backend" OFF)

add_subdirectory(engine)
add_subdirectory(pong)

execute_process(
  COMMAND
    ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_BINARY_DIR}/compile_commands.json
    ${CMAKE_SOURCE_DIR}/compile_commands.json)
