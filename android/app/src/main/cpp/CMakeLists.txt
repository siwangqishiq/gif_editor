cmake_minimum_required(VERSION 3.18.1)

# Declares and names the project.
project("gif_editor")

set(PURPLE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../purple)  #purple源码
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../src)

include_directories(${PURPLE_DIR}/include) #添加include目录
include_directories(${SRC_DIR})

include_directories(${CMAKE_SOURCE_DIR}/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}")

file(GLOB_RECURSE PURPLE_SRC_FILES ${PURPLE_DIR}/*.cpp)
file(GLOB_RECURSE SRC_FILES ${SRC_DIR}/*.cpp)
file(GLOB_RECURSE HEAD_FILES ${SRC_DIR}/*.h)

file(GLOB_RECURSE ALL_SUBDIRS LIST_DIRECTORIES true ${SRC_DIR}/*)
set(SRC_INCLUDE_DIRS "")
foreach(dir ${ALL_SUBDIRS})
        if(IS_DIRECTORY ${dir})
                list(APPEND SRC_INCLUDE_DIRS ${dir})
                message("SRC_INCLUDE_DIRS : ${dir}")
        endif()
endforeach()
include_directories(${SRC_INCLUDE_DIRS})

#message("=== ALL_SOURCES BEFORE REMOVE ===")
#foreach(src ${SRC_FILES})
#        message("${src}")
#endforeach()
#message("=== ALL_SOURCES BEFORE REMOVE END ===")

# 排除特定文件
message("=== START REMOVE ===")
list(REMOVE_ITEM ALL_SOURCES ${PURPLE_DIR}/glad.cpp)
list(REMOVE_ITEM SRC_FILES ${SRC_DIR}/application.cpp ${SRC_DIR}/main.cpp)
list(REMOVE_ITEM HEAD_FILES ${SRC_DIR}/application.h)
list(REMOVE_ITEM SRC_FILES ${SRC_DIR}/main.cpp)
message("=== END     REMOVE ===")

message("=== ALL_SOURCES AFTER REMOVE ===")
foreach(src ${SRC_FILES})
        message("${src}")
endforeach()

add_library( # Sets the name of the library.
        purple
        SHARED
        bridge.cpp
        AndroidApplication.cpp
        ${SRC_FILES}
#        ${SRC_DIR}/test1_app.cpp
#        ${SRC_DIR}/test_render_text.cpp
#        ${SRC_DIR}/test_ui.cpp
#        ${SRC_DIR}/test_ui_img.cpp
#        ${SRC_DIR}/test_ui_text.cpp
#        ${SRC_DIR}/test_particle.cpp
        ${PURPLE_SRC_FILES}
        )

set(opengl_lib GLESv3)

target_link_libraries( # Specifies the target library.
        purple
        avcodec
        avdevice
        avfilter
        avformat
        avutil
        swresample
        swscale
        log
        ${opengl_lib}
        android
        jnigraphics
        )