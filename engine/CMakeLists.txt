cmake_minimum_required(VERSION 3.16)
project(libengine)

set(CMAKE_CXX_STANDARD 17)

set(GCC_COVERAGE_COMPILE_FLAGS "-O2")

if(NOT MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
endif()

set(Vulkan_INCLUDE_DIRS "$ENV{VULKAN_SDK}/Include")
set(Vulkan_LIBRARIES "$ENV{VULKAN_SDK}/Lib")

if(ENGINE_USE_VLK)
  find_package(Vulkan REQUIRED)

  set(GLFW_BUILD_DOCS
      OFF
      CACHE BOOL "" FORCE)
  set(GLFW_BUILD_TESTS
      OFF
      CACHE BOOL "" FORCE)
  set(GLFW_BUILD_EXAMPLES
      OFF
      CACHE BOOL "" FORCE)
  set(GLFW_VULKAN_STATIC
      ON
      CACHE BOOL "" FORCE)
  find_package(glfw3 REQUIRED)
  find_package(glm REQUIRED)
else()
  find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
  find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)
  find_package(SDL2_mixer REQUIRED)
  find_package(SDL2_ttf REQUIRED)
  find_package(SDL2_image REQUIRED)
  include_directories(${SDL2_INCLUDE_DIRS})

  file(GLOB_RECURSE ENGINE_SRC CONFIGURE_DEPENDS
       "${PROJECT_SOURCE_DIR}/src/*.hpp" "${PROJECT_SOURCE_DIR}/src/*.cpp")

  add_library(engine STATIC ${ENGINE_SRC})

  if(TARGET SDL2::SDL2main)
    target_link_libraries(engine PUBLIC SDL2::SDL2main)
  endif()
  target_link_libraries(engine PUBLIC SDL2::SDL2)
  target_link_libraries(engine PUBLIC SDL2_ttf::SDL2_ttf) # ttf
  target_link_libraries(engine PUBLIC SDL2_mixer::SDL2_mixer) # ttf
  target_link_libraries(engine PUBLIC SDL2_image::SDL2_image) # ttf
  # target_link_libraries(engine PUBLIC ${SDL2_LIBRARY} ${SDL2IMAGE_LIBRARY})
  # target_link_libraries(engine PUBLIC ${SDL2_LIBRARY} ${SDL2MIXER_LIBRARY})

  target_include_directories(engine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

endif()
