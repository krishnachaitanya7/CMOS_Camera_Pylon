cmake_minimum_required(VERSION 3.14)
project(cmos_camera_pylon)
set(CMAKE_CXX_STANDARD 14)
include_directories(/opt/pylon5/include)
link_directories(/opt/pylon5/lib64)
add_executable(cmos_camera_pylon main.cpp)
target_link_libraries(cmos_camera_pylon pylonutility pylonbase GenApi_gcc_v3_1_Basler_pylon GCBase_gcc_v3_1_Basler_pylon)

