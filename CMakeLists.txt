cmake_minimum_required(VERSION 3.25)
project(libmatch)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

# 判断系统位数
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(OpenCV_DIR ./3rdpart/ocvm/x64/x64/vc14/staticlib)
    set(ncnn_DIR ./3rdpart/ncnn/x64/lib/cmake/ncnn)
    set(ncnn_INCLUDE_DIRS ./3rdpart/ncnn/x64/include)
else()
    set(OpenCV_DIR ./3rdpart/ocvm/x86/x86/vc14/staticlib)
    set(ncnn_DIR ./3rdpart/ncnn/x86/lib/cmake/ncnn)
    set(ncnn_INCLUDE_DIRS ./3rdpart/ncnn/x86/include)
endif()

find_package(OpenCV REQUIRED)	# 找到opencv库
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(ncnn REQUIRED)    # 找到ncnn库
include_directories(${ncnn_INCLUDE_DIRS})


# Configure CCache if available
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)


include_directories(./3rdpart/configor)

# 添加 defined 以识别导入导出
add_definitions(-DLIBMATCH_EXPORTS)

add_library(libmatch SHARED src/library.cpp
        src/base_algorithm.cpp
        src/base_match.cpp
        src/c_api.cpp
        src/clipper.cpp
        src/pp_ocr.cpp
        src/yolos/yolov57.cpp
        src/dxgi/screenshot.cpp
)
target_link_libraries(libmatch ${OpenCV_LIBRARIES})
target_link_libraries(libmatch ncnn)