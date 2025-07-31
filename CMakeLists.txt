cmake_minimum_required(VERSION 3.10)

project(app)

set(FFMPEG_DIR ".") 


include_directories(${FFMPEG_DIR}/include)
include_directories(purple/include)
include_directories(vender/glfw/include)

add_subdirectory(purple)
aux_source_directory(src SRCS)

if (WIN32)
    message(STATUS "Using Win32 library")
    link_directories(lib/win64)
endif()


add_executable(${PROJECT_NAME} ${SRCS})

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-Wall")
# set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-mwindows")

target_link_libraries(${PROJECT_NAME} purple)
target_link_libraries(${PROJECT_NAME} glfw3)
target_link_libraries(${PROJECT_NAME}
    avformat
    avcodec
    avutil
    swscale
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/dlls"
        "$<TARGET_FILE_DIR:app>"
)
