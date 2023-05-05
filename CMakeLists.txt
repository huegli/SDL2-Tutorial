cmake_minimum_required(VERSION 3.20)

project(sdl2_tutorials)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL2 REQUIRED COMPONENTS SDL2)
# find_package(SDL2_image REQUIRED)
Include(FetchContent)
FetchContent_Declare(
  SDL_image
  GIT_REPOSITORY https://github.com/libsdl-org/SDL_image
  GIT_TAG        release-2.6.3
)

FetchContent_MakeAvailable(SDL_image)

add_subdirectory(Lesson_01)
add_subdirectory(Lesson_02)
add_subdirectory(Lesson_03)
add_subdirectory(Lesson_04)
add_subdirectory(Lesson_05)
add_subdirectory(Lesson_06)
add_subdirectory(Lesson_07)
add_subdirectory(Lesson_08)
add_subdirectory(Lesson_09)
add_subdirectory(Lesson_10)
