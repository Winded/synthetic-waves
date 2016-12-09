project(luajogo)
cmake_minimum_required(VERSION 2.8)

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
    user32
    gdi32
    liblua
    libglew_shared
    SDL2
    SDL2main
)

include_directories(include
    ${LUAJIT_DIR}/src
    ${SDL_DIR}/include
    ${GLEW_DIR}/include
)

add_definitions(-DUSE_OPENGL)

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

