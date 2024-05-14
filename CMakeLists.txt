## --- Project configuration --- ##
cmake_minimum_required(VERSION 3.10)
project(Loraine VERSION 0.1)

## --- Compiler configuration --- ##
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
include_directories("include/")

## --- Packages configuration --- ##
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Boost REQUIRED)

## --- Executable configuration --- ##
add_executable(Loraine "source/main.cpp"
    "source/config.cpp"
    "source/controller.cpp"
    "source/http_server.cpp"
    "source/i2c.cpp"
    "source/utility.cpp"
)
target_link_libraries(Loraine PRIVATE
    fmt::fmt
    spdlog::spdlog
    Boost::boost
    "wiringPi"
)
