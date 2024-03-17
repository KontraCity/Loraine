### --- Initial configuration --- ###
cmake_minimum_required(VERSION 3.10)
project(Loraine VERSION 0.1)

### --- C++ standart configuration --- ###
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

### --- Executable configuration --- ###
add_executable(Loraine "source/main.cpp"
    "source/config.cpp"
    "source/controller.cpp"
)
target_include_directories(Loraine PRIVATE "include/")

### --- Linking configuration --- ###
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(Loraine PRIVATE
    fmt::fmt
    spdlog::spdlog
    "wiringPi"
)
