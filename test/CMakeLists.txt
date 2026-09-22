cmake_minimum_required(VERSION 3.17)
project(sdl_example C CXX)

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
endif() 


# ★ 选哪个示例
#   EXAMPLE=001  → 只生成 ex001（推荐，proj.win32 里干净）
#   EXAMPLE=002  → 只生成 ex002
#   EXAMPLE=all  → 全部生成
#set(EXAMPLE "001" CACHE STRING "Which example to build (001 / 002 / 003 / all)")



add_subdirectory(examples/001_2d_vertex_rotation)
    add_subdirectory(examples/002_2d_ray)
    add_subdirectory(examples/003_n_order_bezier)
    add_subdirectory(examples/004_pixel_instantiation)
    add_subdirectory(examples/005_surface_move_transparent)
    add_subdirectory(examples/006_renderer_move_transparent)
    add_subdirectory(examples/007_01_menu)
    add_subdirectory(examples/008_small_ball_gravity_system)
    add_subdirectory(examples/009_water_ripple)
    add_subdirectory(examples/010_01_motion_streak)
    add_subdirectory(examples/010_02_camera_motion_streak)
    add_subdirectory(examples/011_2d_matrix_and_vertex)
    add_subdirectory(examples/012_sine_matrix_wave)
    add_subdirectory(examples/013_sound_pitching_example)
    add_subdirectory(examples/014_sound_wave) 
 
