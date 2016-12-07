project(luajogo)
cmake_minimum_required(VERSION 2.8)

set(LUAJIT_DIR thirdparty/luajit)
set(GLFW_DIR thirdparty/glfw)
set(GLAD_DIR thirdparty/glad)

add_subdirectory(${LUAJIT_DIR})
add_subdirectory(${GLFW_DIR})

file(GLOB_RECURSE SRC_LIST
    "include/*.h"
    "src/*.c"
    "${GLAD_DIR}/src/*.c"
)
add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(${PROJECT_NAME}
    user32
    gdi32
    liblua
    glfw
)

include_directories(include
    ${LUAJIT_DIR}/src
    ${GLFW_DIR}/include
    ${GLAD_DIR}/include
)

add_custom_command(TARGET luajogo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:liblua>
        $<TARGET_FILE_DIR:luajogo>)

add_custom_command(TARGET luajogo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:glfw>
        $<TARGET_FILE_DIR:luajogo>)

