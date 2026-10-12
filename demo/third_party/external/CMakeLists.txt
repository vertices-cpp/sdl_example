# third_party/external/CMakeLists.txt
cmake_minimum_required(VERSION 3.17)
project(external)

# ============================================================
# 1. 各子库（顺序：被依赖的在前）
# ============================================================

# ---- zlib（minizip 依赖它）----
add_subdirectory(zlib)

# ---- tinyxml2 ----
add_subdirectory(tinyxml2)

# ---- minizip（依赖 zlib）----
add_subdirectory(minizip)

# ---- edtaa3func ----
add_subdirectory(edtaa3func)

# ---- FreeType（全局一份，关掉不需要的依赖）----
set(FT_DISABLE_ZLIB     TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2    TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG      TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI   TRUE CACHE BOOL "" FORCE)
set(FT_DISABLE_HVF      TRUE CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS   OFF  CACHE BOOL "" FORCE)

add_subdirectory(freetype ${CMAKE_BINARY_DIR}/freetype)

# ---- Chipmunk（只静态，不给 binary dir 会污染源码目录）----
set(BUILD_DEMOS    OFF CACHE BOOL "" FORCE)
set(INSTALL_DEMOS  OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED   OFF CACHE BOOL "" FORCE)
set(BUILD_STATIC   ON  CACHE BOOL "" FORCE)
set(INSTALL_STATIC OFF CACHE BOOL "" FORCE)

add_subdirectory(Chipmunk2D ${CMAKE_BINARY_DIR}/Chipmunk2D)

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

# ============================================================
# 3. 统一入口（INTERFACE 库，只传 include + link）
# ============================================================
add_library(external INTERFACE)

target_include_directories(external INTERFACE
    # Chipmunk2D 用老式 include_directories，不传播，手动加
    "${CMAKE_CURRENT_SOURCE_DIR}/Chipmunk2D/include"
    ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(external INTERFACE
    edtaa3func
    zlib
    tinyxml2
    minizip
    freetype
    chipmunk_static
    ${SDL2_LIBRARIES}
)

# ============================================================
# 4. VS 分组
# ============================================================
foreach(tgt
    edtaa3func
    zlib
    tinyxml2
    minizip
    freetype
    chipmunk_static
)
    if(TARGET ${tgt})
        set_target_properties(${tgt} PROPERTIES FOLDER "External")
    endif()
endforeach()