project(luajogo)
cmake_minimum_required(VERSION 3.1)

set(LUAJIT_DIR thirdparty/luajit)
set(SDL_DIR thirdparty/sdl)
set(GLEW_DIR thirdparty/glew)

add_subdirectory(${LUAJIT_DIR})
add_subdirectory(${SDL_DIR})
add_subdirectory(${GLEW_DIR})

file(GLOB_RECURSE SRC_LIST
    "include/*.h"
    "src/*.c"
)
add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(${PROJECT_NAME}
    liblua
    libglew_shared
    SDL2
    SDL2main
    shlwapi
)

include_directories(include
    ${LUAJIT_DIR}/src
    ${SDL_DIR}/include
    ${GLEW_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/${LUAJIT_DIR}
)

add_definitions(-DUSE_OPENGL)
add_definitions(-D_USE_MATH_DEFINES)

if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
        set (CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")
    endif ()
else ()
    set (CMAKE_C_STANDARD 99)
endif ()

add_custom_command(TARGET luajogo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:liblua>
        $<TARGET_FILE_DIR:luajogo>)

add_custom_command(TARGET luajogo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:libglew_shared>
        $<TARGET_FILE_DIR:luajogo>)

add_custom_command(TARGET luajogo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL2>
        $<TARGET_FILE_DIR:luajogo>)

