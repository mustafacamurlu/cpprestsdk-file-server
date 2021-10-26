cmake_minimum_required(VERSION 3.10)
project(FileServer)
set(CMAKE_CXX_STANDARD 17)
add_executable(FileServer main.cpp base_controller.cpp rest_server.cpp)
target_link_libraries(FileServer cpprest pthread crypto ssl boost_system)

