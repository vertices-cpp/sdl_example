cmake_minimum_required(VERSION 3.17)
project(sdl_example C CXX)

if(MSVC)
    add_compile_options(/utf-8 /MP)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# ★ 全局资源镜像，只做一次
if(EXISTS "${CMAKE_SOURCE_DIR}/Resources")
    add_custom_target(SyncRes ALL
        COMMAND ${CMAKE_COMMAND} -E rm -rf
            "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/Resources"
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/Resources"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/Resources"
            "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/Resources"
        COMMENT "Mirror Resources -> $<CONFIG>"
        VERBATIM)

    set_target_properties(SyncRes PROPERTIES FOLDER "Utils")
endif()

# ============================================================
# 外部库（FreeType / zlib / ... 统一入口）
# ============================================================
add_subdirectory(third_party/external)

# ============================================================
# 公共头（独立项目，只列一次）
# ============================================================
add_subdirectory(examples/common)

# ============================================================
# 所有示例
# ============================================================
add_subdirectory(examples/001_2d_vertex_rotation)
add_subdirectory(examples/002_2d_ray)
add_subdirectory(examples/003_n_order_bezier)
add_subdirectory(examples/004_pixel_instantiation)
add_subdirectory(examples/005_surface_move_transparent)
add_subdirectory(examples/006_renderer_move_transparent)

add_subdirectory(examples/007_small_ball_gravity_system)
add_subdirectory(examples/008_water_ripple)
add_subdirectory(examples/009_motion_streak)
add_subdirectory(examples/010_camera_motion_streak)
add_subdirectory(examples/011_2d_matrix_and_vertex)
add_subdirectory(examples/012_sine_matrix_wave)
add_subdirectory(examples/013_sound_pitching_example)
add_subdirectory(examples/014_sound_engine)
add_subdirectory(examples/015_sound_wave)
add_subdirectory(examples/016_font)
add_subdirectory(examples/017_menu)
add_subdirectory(examples/018_menu_and_item)
add_subdirectory(examples/019_chipmunk2d_test)