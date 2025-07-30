cmake_minimum_required(VERSION 3.10)

project(gif_editor)

set(APP_NAME "app")

set(FFMPEG_DIR ".") 

include_directories(${FFMPEG_DIR}/include)
link_directories(${FFMPEG_DIR}/lib)

aux_source_directory(. SRCS)

add_executable(${APP_NAME} ${SRCS})

target_link_libraries(${APP_NAME}
    avformat
    avcodec
    avutil
    swscale
)

add_custom_command(TARGET ${APP_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/dlls"
        "$<TARGET_FILE_DIR:app>"
)
