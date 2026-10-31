# examples/common/CMakeLists.txt
cmake_minimum_required(VERSION 3.17)
project(common CXX)

# ============================================================
# 2. SDL2（预编译库，不走 add_subdirectory）
# ============================================================
get_filename_component(_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
set(SDL2_ROOT "${_ROOT}/third_party/SDL2")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_DIR "x64")
else()
    set(ARCH_DIR "x86")
endif()

# 需要哪些模块就填哪些
set(SDL2_MODULES
    "SDL2-2.0.20"
    # "SDL2_image-2.0.5"
    # "SDL2_mixer-2.0.4"
    # "SDL2_ttf-2.0.18"
    # "SDL2_net-2.0.1"
)

set(SDL2_INCLUDE_DIRS)
set(SDL2_LIBRARIES)
foreach(module ${SDL2_MODULES})
    list(APPEND SDL2_INCLUDE_DIRS "${SDL2_ROOT}/${module}/include")
    file(GLOB _libs CONFIGURE_DEPENDS
        "${SDL2_ROOT}/${module}/lib/${ARCH_DIR}/*.lib")
    list(APPEND SDL2_LIBRARIES ${_libs})
endforeach()

target_include_directories(external INTERFACE
   
    ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(external INTERFACE
  
    ${SDL2_LIBRARIES}
)
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

# ★ 加在这里，文件最末尾
if(MSVC)
    target_compile_options(common PRIVATE /EHsc)
endif()