cmake_minimum_required(VERSION 3.10)
project(file_server)
set(CMAKE_CXX_STANDARD 17)
add_executable(file_server file_server.cpp)
target_include_directories(file_server PUBLIC ${LIBCTRL_CMAKE_ROOT_DIR}/include)
target_link_libraries(file_server cpprest pthread crypto ssl boost_system libcontroller)

