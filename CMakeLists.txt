cmake_minimum_required(VERSION 3.5)
project(nya-sweeper C)
set (CMAKE_C_STANDARD 23)

include(FetchContent)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

add_subdirectory(third-party/raylib)

file (GLOB SRC_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/source/*.c")
set (PROJECT_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/include/" "${CMAKE_CURRENT_LIST_DIR}/third-party/raylib/src")

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${SRC_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE})
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="./assets/")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_LIST_DIR}/assets
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)