cmake_minimum_required(VERSION 3.5)
project(Fly VERSION 0.0.1)

include(CTest)
enable_testing()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_FLAGS "-Wall -Wextra")
set (CMAKE_CXX_FLAGS_DEBUG "-g")
set (CMAKE_CXX_FLAGS_RELEASE "-O2")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

include(FetchContent)
FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	bf71a834948186f4097caa076cd2663c69a10e1e #refs/tags/1.0.1
)
FetchContent_MakeAvailable(glm)

if(WIN32)
    set(SDL2_DIR "${PROJECT_SOURCE_DIR}/lib/SDL2/cmake")
endif()

find_package(SDL2 REQUIRED)
find_package(OpenMP REQUIRED)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mconsole")

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(${PROJECT_NAME}
    "src/glad.c"
    ${SOURCES}
)
target_link_libraries(${PROJECT_NAME} 
    SDL2::SDL2main 
    SDL2::SDL2
    glm::glm
    OpenMP::OpenMP_CXX
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/lib"
)

set_target_properties(Fly PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}
)

if(WIN32)
    add_custom_command(TARGET Fly POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:Fly> $<TARGET_FILE_DIR:Fly>
        COMMAND_EXPAND_LISTS
    )
endif()
