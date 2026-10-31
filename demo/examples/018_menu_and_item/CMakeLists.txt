cmake_minimum_required(VERSION 3.17)

# ============================================================
# ★ 唯一要改的地方：项目名
# ============================================================
set(APP_NAME 018_menu_and_item)
# ============================================================

project(${APP_NAME} C CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if(MSVC)
    add_compile_options(/utf-8 /MP /W3 /Gm-)
    add_compile_options($<$<CONFIG:Release>:/O2>)
endif()

# ---------------- SDL2 ----------------
get_filename_component(_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
 
set(SDL2_ROOT "${_ROOT}/third_party/SDL2")
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_DIR "x64")
else()
    set(ARCH_DIR "x86")
endif()
 
# ============================================================
# ★ FreeType 由顶层 add_subdirectory(third_party/freetype) 定义
#   这里直接链接 freetype target 即可
# ============================================================

# ---------------- 自动扫描 example 自己的源文件 ----------------
file(GLOB_RECURSE _srcs CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)
list(FILTER _srcs EXCLUDE REGEX "/build[^/]*/")
list(FILTER _srcs EXCLUDE REGEX "/\\.vs/")
list(FILTER _srcs EXCLUDE REGEX "/out/")
list(FILTER _srcs EXCLUDE REGEX "/proj\\.win32[^/]*/")

# ---------------- 目标 ----------------
add_executable(${APP_NAME} WIN32 ${_srcs})

target_include_directories(${APP_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(${APP_NAME} PRIVATE
    common
)

# ---------------- 拷 DLL ----------------
if(WIN32)
    file(GLOB _dlls "${SDL2_ROOT}/*/lib/${ARCH_DIR}/*.dll")
    foreach(_dll ${_dlls})
        get_filename_component(_dllname ${_dll} NAME)
        add_custom_command(TARGET ${APP_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${_dll}" "$<TARGET_FILE_DIR:${APP_NAME}>/${_dllname}"
            COMMENT "Copy ${_dllname}")
    endforeach()
endif()

add_dependencies(${APP_NAME} SyncRes)

# ---------------- VS 分组 ----------------
if(MSVC)
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
                 PREFIX "Source Files"
                 FILES ${_srcs})
endif()

set_target_properties(${APP_NAME} PROPERTIES FOLDER "Test")