cmake_minimum_required(VERSION 3.10)
project(libcontroller VERSION 0.0.1 DESCRIPTION "controller layer for cpprestsdk")
set(CMAKE_CXX_STANDARD 17)

SET(LIBCTRL_CMAKE_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_library(libcontroller SHARED
    libcontroller/src/rest_server.cpp
    libcontroller/src/base_controller.cpp
    libcontroller/src/jwt_validator.cpp
)

set_target_properties(libcontroller PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(libcontroller PROPERTIES SOVERSION 1)

set_target_properties(libcontroller PROPERTIES PUBLIC_HEADER
    include/base_controller.hpp
    include/rest_server.hpp
    include/jwt_validator.hpp
)

#add_executable(FileServer main.cpp base_controller.cpp rest_server.cpp)

target_include_directories(libcontroller PRIVATE
    include
    libcontroller/inc
    libcontroller/inc/jwt-cpp
    libcontroller/inc/nlohmann
    libcontroller/inc/picojson
)

target_link_libraries(libcontroller cpprest pthread crypto ssl boost_system)

add_subdirectory(examples)