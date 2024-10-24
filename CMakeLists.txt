cmake_minimum_required(VERSION 3.20)
project(Fly VERSION 0.0.1)

include(CTest)
enable_testing()

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_FLAGS "-Wall -Wextra")
set (CMAKE_CXX_FLAGS_DEBUG "-g")
set (CMAKE_CXX_FLAGS_RELEASE "-O2")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

set(SDL2_DIR "C:/SDL/SDL2/cmake")
set(SDL2_image_DIR "C:/SDL/SDL2_image/cmake")
set(SDL2_ttf_DIR "C:/SDL/SDL2_ttf/cmake")

#list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(OpenMP REQUIRED)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mconsole")

add_subdirectory(include/SDL_FontCache)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} 
    SDL2::SDL2main 
    SDL2_image::SDL2_image 
    SDL2_ttf::SDL2_ttf
    glm::glm
    SDL_FontCache
    #OpenMP::OpenMP_CXX
)

target_include_directories(${PROJECT_NAME} PUBLIC
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/include/SDL_FontCache"
)

if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${PROJECT_SOURCE_DIR}/lib"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()
