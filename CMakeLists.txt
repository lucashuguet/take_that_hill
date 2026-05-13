cmake_minimum_required (VERSION 3.20)
project (${APP_NAME})
set(CMAKE_CXX_STANDARD 23)

set(SOURCES src/main.cpp src/game.cpp src/utils.cpp)
set(HEADERS lib/game.h lib/utils.h)

add_executable(${APP_NAME} ${SOURCES} ${HEADER_FILES})
target_include_directories(${APP_NAME} PRIVATE lib)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${APP_NAME} PRIVATE -O0 -g3
        -ffile-prefix-map=${CMAKE_SOURCE_DIR}=.)
endif()

install(TARGETS ${APP_NAME} DESTINATION bin)
