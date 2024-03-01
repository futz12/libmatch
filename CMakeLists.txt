cmake_minimum_required(VERSION 3.27)
project(libmatch)

set(CMAKE_CXX_STANDARD 17)

# 判断系统位数
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(OpenCV_DIR ./3rdpart/ocvm/x64/x64/vc14/staticlib)
else()
    set(OpenCV_DIR ./3rdpart/ocvm/x86/x86/vc14/staticlib)
endif()

find_package(OpenCV REQUIRED)	# 找到opencv库
include_directories(${OpenCV_INCLUDE_DIRS})

# Configure CCache if available
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

# 添加 defined 以识别导入导出
add_definitions(-DLIBMATCH_EXPORTS)

add_library(libmatch SHARED src/library.cpp
        src/base_algorithm.cpp
        src/base_match.cpp
        src/c_api.cpp
)
target_link_libraries(libmatch ${OpenCV_LIBRARIES})