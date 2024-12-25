cmake_minimum_required(VERSION 3.10)

project(uniclipboard LANGUAGES C CXX)

set(EXECUTABLE_NAME ${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Command.json for nvim, this will not work on Windows unless Ninja is used
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

#set(CMAKE_BUILD_TYPE Release)

# ------main-------

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

set(SOURCES
    src/main.cpp 
    src/install.cpp 
)

if(WIN32)
    add_compile_definitions(WINDOWS_BUILD)
elseif(APPLE)
  add_compile_definitions(MACOS_BUILD)
elseif(ANDROID)
    add_compile_definitions(ANDROID_BUILD)
endif()

add_executable(${EXECUTABLE_NAME} ${SOURCES})

target_include_directories(${EXECUTABLE_NAME} PRIVATE include)

