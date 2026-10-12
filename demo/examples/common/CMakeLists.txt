# examples/common/CMakeLists.txt
cmake_minimum_required(VERSION 3.17)
project(common CXX)

# ============================================================
# 源文件（递归扫描）
# ============================================================
file(GLOB_RECURSE COMMON_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

add_library(common STATIC ${COMMON_SOURCES})

# ============================================================
# include：只暴露自己的头
# ============================================================
target_include_directories(common PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# ============================================================
# 第三方：只链 external，不直接碰 chipmunk / freetype / SDL2
# ============================================================
target_link_libraries(common PUBLIC
    external
)

# ============================================================
# VS 分组
# ============================================================
if(MSVC)
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
                 PREFIX "Source Files"
                 FILES ${COMMON_SOURCES})
endif()

set_target_properties(common PROPERTIES FOLDER "Common")

# ============================================================
# 调试宏
# ============================================================
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

if(MSVC)
    target_compile_options(common PUBLIC /MP)
endif()