cmake_minimum_required(VERSION 3.20)

project(sdl2_tutorials)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL2 REQUIRED COMPONENTS SDL2)

Include(FetchContent)

FetchContent_Declare(
  SDL_image
  GIT_REPOSITORY https://github.com/libsdl-org/SDL_image
  GIT_TAG        release-2.6.3
)
FetchContent_MakeAvailable(SDL_image)

FetchContent_Declare(
  SDL_ttf
  GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf
  GIT_TAG        release-2.20.2
)
FetchContent_MakeAvailable(SDL_ttf)

FetchContent_Declare(
  SDL_mixer
  GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer
  GIT_TAG        release-2.6.3
)
FetchContent_MakeAvailable(SDL_mixer)

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
add_subdirectory(Lesson_11)
add_subdirectory(Lesson_12)
add_subdirectory(Lesson_13)
add_subdirectory(Lesson_14)
add_subdirectory(Lesson_15)
add_subdirectory(Lesson_16)
add_subdirectory(Lesson_17)
add_subdirectory(Lesson_18)
add_subdirectory(Lesson_19)
add_subdirectory(Lesson_20)
add_subdirectory(Lesson_21)
