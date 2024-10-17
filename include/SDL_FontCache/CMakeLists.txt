cmake_minimum_required(VERSION 3.15.0)
project(SDL_FontCache VERSION 0.10.0 LANGUAGES C)

set(SDL2_DIR "C:/SDL/SDL2/cmake")
set(SDL2_ttf_DIR "C:/SDL/SDL2_ttf/cmake")
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

add_library(SDL_FontCache
    SDL_FontCache.h
    SDL_FontCache.c
)
target_link_libraries(SDL_FontCache SDL2::SDL2main SDL2::SDL2 SDL2_ttf::SDL2_ttf)