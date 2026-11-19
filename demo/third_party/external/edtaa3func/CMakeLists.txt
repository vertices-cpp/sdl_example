# ============================================================
# Path: game/orange/external/edtaa3func/
# ============================================================

cmake_minimum_required(VERSION 3.17)

set(lib_name edtaa3func)
set(target_name ${lib_name})

project(${lib_name} C CXX)

# ============================================================
# Source files
# ============================================================
set(${target_name}_src
    ${CMAKE_CURRENT_SOURCE_DIR}/edtaa3func.cpp
)

set(${target_name}_header
    ${CMAKE_CURRENT_SOURCE_DIR}/edtaa3func.h
)

# ============================================================
# Library
# ============================================================
add_library(${target_name} STATIC
    ${${target_name}_header}
    ${${target_name}_src}
)

target_include_directories(${target_name} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# ============================================================
# Output
# ============================================================
set_target_properties(${target_name} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    FOLDER "External"
)

# ============================================================
# VS grouping
# ============================================================
if(MSVC OR XCODE)
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
                 PREFIX "Source Files"
                 FILES ${${target_name}_header} ${${target_name}_src})
endif()