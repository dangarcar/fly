cmake_minimum_required(VERSION 3.15.0)
project(SDL_FontCache VERSION 0.10.0 LANGUAGES C)

add_library(SDL_FontCache
    SDL_FontCache.h
    SDL_FontCache.c
)
target_link_libraries(SDL_FontCache SDL2::SDL2main SDL2::SDL2 SDL2_ttf::SDL2_ttf)