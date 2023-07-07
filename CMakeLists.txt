cmake_minimum_required(VERSION 3.20)

project(sdl2_tutorials)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL2 REQUIRED COMPONENTS SDL2)
find_package(SDL2_IMAGE REQUIRED COMPONENTS SDL_image)
find_package(SDL2_TTF REQUIRED COMPONENTS SDL_ttf)
find_package(SDL2_MIXER REQUIRED COMPONENTS SDL_mixer)

# Include(FetchContent)
#
# FetchContent_Declare(
#   SDL_ttf
#   GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf
#   GIT_TAG        release-2.20.2
# )
# FetchContent_MakeAvailable(SDL_ttf)
#
# FetchContent_Declare(
#   SDL_mixer
#   GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer
#   GIT_TAG        release-2.6.3
# )
# FetchContent_MakeAvailable(SDL_mixer)

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
add_subdirectory(Lesson_22)
add_subdirectory(Lesson_23)
add_subdirectory(Lesson_24)
add_subdirectory(Lesson_25)
add_subdirectory(Lesson_26)
add_subdirectory(Lesson_27)
add_subdirectory(Lesson_28)
add_subdirectory(Lesson_29)
add_subdirectory(Lesson_30)
add_subdirectory(Lesson_31)
add_subdirectory(Lesson_32)
add_subdirectory(Lesson_33)
add_subdirectory(Lesson_34)
add_subdirectory(Lesson_35)
add_subdirectory(Lesson_36)
add_subdirectory(Lesson_37)
add_subdirectory(Lesson_38)
add_subdirectory(Lesson_39)
add_subdirectory(Lesson_40)
add_subdirectory(Lesson_41)
add_subdirectory(Lesson_42)
add_subdirectory(Lesson_43)
add_subdirectory(Lesson_44)
add_subdirectory(Lesson_45)
add_subdirectory(Lesson_46)
add_subdirectory(Lesson_47)
add_subdirectory(Lesson_48)
add_subdirectory(Lesson_49)
