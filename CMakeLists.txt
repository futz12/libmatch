cmake_minimum_required(VERSION 3.25)
project(libmatch)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

# if reldebug is enabled, set the optimization level to disable
if (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Od")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Od")
endif ()


option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_EXAMPLES "Build examples" ON)

# Windows
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # 判断系统位数
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(OpenCV_DIR ./3rdpart/windows/ocvm/x64/x64/vc15/staticlib)
        set(ncnn_DIR ./3rdpart/windows/ncnn/x64/lib/cmake/ncnn)
        set(ncnn_INCLUDE_DIRS ./3rdpart/windows/ncnn/x64/include)
    else ()
        set(OpenCV_DIR ./3rdpart/windows/ocvm/x86/x86/vc15/staticlib)
        set(ncnn_DIR ./3rdpart/windows/ncnn/x86/lib/cmake/ncnn)
        set(ncnn_INCLUDE_DIRS ./3rdpart/windows/ncnn/x86/include)
    endif ()
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(OpenCV_DIR ./3rdpart/linux/ocvm/lib/cmake/opencv4)
    set(ncnn_DIR ./3rdpart/linux/ncnn/lib/cmake/ncnn)
    set(ncnn_INCLUDE_DIRS ./3rdpart/linux/ncnn/include)
endif ()

find_package(OpenCV REQUIRED) # 找到opencv库
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(ncnn REQUIRED)    # 找到ncnn库
include_directories(${ncnn_INCLUDE_DIRS})


# Configure CCache if available
find_program(CCACHE_FOUND ccache)
if (CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif (CCACHE_FOUND)

# 添加 defined 以识别导入导出
add_definitions(-DLIBMATCH_EXPORTS)

if (BUILD_SHARED_LIBS)
    add_library(libmatch SHARED src/library.cpp
            src/base_algorithm.cpp
            src/base_match.cpp
            src/c_api.cpp
            src/clipper.cpp
            src/pp_ocr.cpp
            src/ddddrec.cpp
    )
else ()
    add_library(libmatch STATIC src/library.cpp
            src/base_algorithm.cpp
            src/base_match.cpp
            src/c_api.cpp
            src/clipper.cpp
            src/pp_ocr.cpp
            src/ddddrec.cpp
    )
endif ()

if(BUILD_EXAMPLES)
    include_directories(src)
    add_executable(libmatch_example_ddddrec SDK/c_demo/ddddrec.cpp
            SDK/c_demo/feat.cpp)
    target_link_libraries(libmatch_example_ddddrec libmatch)

    add_executable(libmatch_example_feat SDK/c_demo/feat.cpp)
    target_link_libraries(libmatch_example_feat libmatch)
endif ()

target_link_libraries(libmatch ${OpenCV_LIBRARIES})
target_link_libraries(libmatch ncnn)