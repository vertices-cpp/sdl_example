cmake_minimum_required(VERSION 3.17)

# ============================================================
# ★ 唯一要改的地方：项目名
# ============================================================
set(APP_NAME 015_font)
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

# 015_font 只需要核心 SDL2（不用 image / mixer / ttf / net）
set(SDL2_MODULES
    "SDL2-2.0.20"
)

set(SDL2_INCLUDE_DIRS)
set(SDL2_LIBRARIES)
foreach(module ${SDL2_MODULES})
    list(APPEND SDL2_INCLUDE_DIRS "${SDL2_ROOT}/${module}/include")
    file(GLOB _libs "${SDL2_ROOT}/${module}/lib/${ARCH_DIR}/*.lib")
    list(APPEND SDL2_LIBRARIES ${_libs})
endforeach()

# ============================================================
# ★ FreeType：用官方 CMakeLists，禁用所有可选依赖
# ============================================================
# 强制关闭 zlib / bzip2 / png / harfbuzz / brotli / HVF，
# 让 freetype 走内建实现，零外部依赖。
set(FT_DISABLE_ZLIB     TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2    TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG      TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI   TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_HVF      TRUE CACHE BOOL "" FORCE)

# 编译静态库（不要 DLL，省得拷贝）
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# 把 freetype 挂到 VS 里的 "third_party" 分组
set(FREETYPE_FOLDER "third_party")
add_subdirectory(freetype)
set_target_properties(freetype PROPERTIES FOLDER "${FREETYPE_FOLDER}")

# ---------------- 自动扫描 example 自己的源文件 ----------------
file(GLOB_RECURSE _srcs CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

# ★ 关键：把 freetype 子树排除，否则它的源文件会被重复编译
list(FILTER _srcs EXCLUDE REGEX "/freetype/")
list(FILTER _srcs EXCLUDE REGEX "/build[^/]*/")
list(FILTER _srcs EXCLUDE REGEX "/\\.vs/")
list(FILTER _srcs EXCLUDE REGEX "/out/")
list(FILTER _srcs EXCLUDE REGEX "/proj\\.win32[^/]*/")

# ---------------- 目标 ----------------
add_executable(${APP_NAME} WIN32 ${_srcs})

target_include_directories(${APP_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    #"${CMAKE_SOURCE_DIR}/examples/common"
    ${SDL2_INCLUDE_DIRS}
)

# 链接 SDL2 + freetype
# freetype target 的 PUBLIC include 会带着 <ft2build.h> 一起进来，
# 所以 OGFontFreeType.cpp 里 #include <ft2build.h> 能直接找到
target_link_libraries(${APP_NAME} PRIVATE
	 common 
    ${SDL2_LIBRARIES}
    freetype
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

set_target_properties(${APP_NAME} PROPERTIES FOLDER "Examples")