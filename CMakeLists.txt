cmake_minimum_required(VERSION 3.14)
project(cmos_camera_pylon)
set(CMAKE_CXX_STANDARD 14)
find_package(OpenCV REQUIRED)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -gdwarf-4")
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(/opt/pylon5/include)
link_directories(/opt/pylon5/lib64)
add_executable(cmos_camera_pylon main.cpp)
add_executable(pylon_bin pylon_camera_bin.cpp)
target_link_libraries(cmos_camera_pylon pylonutility pylonbase GenApi_gcc_v3_1_Basler_pylon GCBase_gcc_v3_1_Basler_pylon ${OpenCV_LIBS})
target_link_libraries(pylon_bin pylonutility pylonbase GenApi_gcc_v3_1_Basler_pylon GCBase_gcc_v3_1_Basler_pylon ${OpenCV_LIBS})


