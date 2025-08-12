cmake_minimum_required(VERSION 3.15...4.0)

project(Floorplan LANGUAGES CXX)

add_library(fplib STATIC src/fplib.cpp include/module.h include/floorplanner.h)
target_include_directories(fplib PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_features(fplib PUBLIC cxx_std_11)

add_executable(fp apps/fp.cpp)
target_link_libraries(fp PUBLIC fplib)