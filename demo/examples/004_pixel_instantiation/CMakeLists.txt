cmake_minimum_required(VERSION 3.17)

# ============================================================
# ★ 唯一要改的地方：项目名
# ============================================================
set(APP_NAME 004_pixel_instantiation)
# ============================================================

project(${APP_NAME} C CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 17)
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
message(STATUS "[${APP_NAME}] SDL2 arch: ${ARCH_DIR}")

set(SDL2_MODULES
    "SDL2-2.0.20"
    "SDL2_image-2.0.5"
    "SDL2_mixer-2.0.4"
    "SDL2_ttf-2.0.18"
    "SDL2_net-2.0.1"
)

set(SDL2_INCLUDE_DIRS)
set(SDL2_LIBRARIES)
foreach(module ${SDL2_MODULES})
    list(APPEND SDL2_INCLUDE_DIRS "${SDL2_ROOT}/${module}/include")
    file(GLOB _libs "${SDL2_ROOT}/${module}/lib/${ARCH_DIR}/*.lib")
    list(APPEND SDL2_LIBRARIES ${_libs})
endforeach()

# ---------------- 自动扫描源文件 ----------------
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
    #"${CMAKE_SOURCE_DIR}/examples/common" 
    ${SDL2_INCLUDE_DIRS})
target_link_libraries(${APP_NAME} PRIVATE
     common           
   ${SDL2_LIBRARIES})

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