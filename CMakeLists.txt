cmake_minimum_required(VERSION 3.0)
project(behold C CXX)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

# Adding Raylib
include(FetchContent)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games

set(CUSTOMIZE_BUILD ON)

set(SUPPORT_MODULE_RMODELS   OFF CACHE BOOL "" FORCE) # don't build 3d models
set(SUPPORT_MODULE_RAUDIO    OFF CACHE BOOL "" FORCE) # don't build audio

set(SUPPORT_FILEFORMAT_BMP ON)
set(SUPPORT_FILEFORMAT_TGA ON)
set(SUPPORT_FILEFORMAT_JPG ON)
set(SUPPORT_FILEFORMAT_GIF ON)
set(SUPPORT_FILEFORMAT_QOI ON)
set(SUPPORT_FILEFORMAT_PSD ON)
set(SUPPORT_FILEFORMAT_PKM ON)
set(SUPPORT_FILEFORMAT_PVR ON)

FetchContent_Declare(raylib GIT_REPOSITORY https://github.com/raysan5/raylib.git GIT_TAG master)
FetchContent_MakeAvailable(raylib)

# Adding our source files
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/sources/*.cpp") 
set(PROJECT_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/sources/") # Define PROJECT_INCLUDE to be the path to the include directory of the project

IF (WIN32)
    list(REMOVE_ITEM PROJECT_SOURCES "${CMAKE_CURRENT_LIST_DIR}/sources/null_platform.cpp")
ELSE()
    list(REMOVE_ITEM PROJECT_SOURCES "${CMAKE_CURRENT_LIST_DIR}/sources/win_platform.cpp")
ENDIF()

message("Compiling files ${PROJECT_SOURCES}")

# Declaring our executable
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE})
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# Setting ASSETS_PATH
target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/") # Set the asset path macro to the absolute path on the dev machine
#target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="./assets") # Set the asset path macro in release mode to a relative path that assumes the assets folder is in the same directory as the game executable
