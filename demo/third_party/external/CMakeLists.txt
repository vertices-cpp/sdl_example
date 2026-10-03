#[[ 路径: orange/external/CMakeLists.txt ]]
cmake_minimum_required(VERSION 3.17)
project(external)

add_subdirectory(zlib)
add_subdirectory(tinyxml2)
add_subdirectory(minizip)
add_subdirectory(freetype)
add_subdirectory(edtaa3func)

add_library(external INTERFACE)
target_link_libraries(external INTERFACE edtaa3func zlib tinyxml2 minizip freetype)

set_target_properties(edtaa3func  zlib tinyxml2 minizip freetype PROPERTIES FOLDER "External")