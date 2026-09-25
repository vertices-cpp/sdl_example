cmake_minimum_required(VERSION 3.17)
project(common CXX)

# 扫描所有源文件 + 头文件
file(GLOB COMMON_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

# ★ STATIC，.cpp 会被真正编译
add_library(common STATIC ${COMMON_SOURCES})

# include 路径传给链接者
target_include_directories(common PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# ============================================================
# SDL2 依赖（common 里用到 SDL_xxx 时加）
# ============================================================
get_filename_component(_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
set(SDL2_ROOT "${_ROOT}/third_party/SDL2")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_DIR "x64")
else()
    set(ARCH_DIR "x86")
endif()

set(SDL2_MODULES
    "SDL2-2.0.20"
  #  "SDL2_image-2.0.5"
  #   "SDL2_mixer-2.0.4"
  #   "SDL2_ttf-2.0.18"
  #   "SDL2_net-2.0.1"
)

target_include_directories(common PUBLIC
    "${SDL2_ROOT}/SDL2-2.0.20/include"
    "${SDL2_ROOT}/SDL2_image-2.0.5/include"
    "${SDL2_ROOT}/SDL2_mixer-2.0.4/include"
    "${SDL2_ROOT}/SDL2_ttf-2.0.18/include"
    "${SDL2_ROOT}/SDL2_net-2.0.1/include"
)

# 链接 SDL2 的 .lib
file(GLOB _sdl2_libs
    "${SDL2_ROOT}/*/lib/${ARCH_DIR}/*.lib")
target_link_libraries(common PUBLIC ${_sdl2_libs})

# VS 分组
if(MSVC)
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
                 PREFIX "Source Files"
                 FILES ${COMMON_SOURCES})
endif()

set_target_properties(common PROPERTIES FOLDER "Common")

# examples/common/CMakeLists.txt 末尾

# 调试宏
target_compile_definitions(common PUBLIC
    $<$<CONFIG:Debug>:ORANGE_DEBUG=1>
)

# Windows 平台宏
if(WIN32)
    target_compile_definitions(common PUBLIC
        WIN32 _WIN32 _WINDOWS UNICODE _UNICODE
        _CRT_SECURE_NO_WARNINGS _SCL_SECURE_NO_WARNINGS
    )
endif()

# MSVC 编译选项
if(MSVC)
    target_compile_options(common PUBLIC /MP)
endif()