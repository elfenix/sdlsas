cmake_minimum_required(VERSION 3.16)

project(sdlsas)

find_package(sdl2)
find_package(sdl2-ttf)
find_package(sdl2-image)
find_package(sdl2-mixer)
#find_package(SDL2_image)
#find_package(SDL2_mixer)

add_executable(sdlsas)

add_subdirectory(src)

target_include_directories(sdlsas PUBLIC "glad/include")
target_sources(sdlsas PUBLIC "glad/src/glad.c")

target_link_libraries(sdlsas PUBLIC SDL2::SDL2 SDL2::SDL2_ttf SDL2::SDL2_image SDL2::SDL2_mixer)
