cmake_minimum_required(VERSION 3.17)

set(APP_NAME 021_ui)
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

# ---------------- 自动扫描示例自己的源文件 ----------------
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
add_executable(${APP_NAME} ${_srcs})

target_include_directories(${APP_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# 只链 common，所有第三方（SDL2 / Chipmunk / freetype）自动从 common -> external 传过来
target_link_libraries(${APP_NAME} PRIVATE
    common
)

# ---------------- 拷 SDL2 DLL ----------------
get_filename_component(_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
set(SDL2_ROOT "${_ROOT}/third_party/SDL2")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_DIR "x64")
else()
    set(ARCH_DIR "x86")
endif()

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

# ---- 拷 websockets.dll（在示例 add_subdirectory 之后）----
if(WIN32 AND TARGET websockets_shared)
    add_custom_command(TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:websockets_shared>"
            "$<TARGET_FILE_DIR:${APP_NAME}>/websockets.dll"
        COMMENT "Copy websockets.dll")
endif()

if(TARGET SyncRes)
    add_dependencies(${APP_NAME} SyncRes)
endif()

if(MSVC)
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
                 PREFIX "Source Files"
                 FILES ${_srcs})
endif()

set_target_properties(${APP_NAME} PROPERTIES FOLDER "Test")