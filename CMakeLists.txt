cmake_minimum_required(VERSION 3.28)
project(Oasis)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(SFML 2.6 COMPONENTS graphics window system audio REQUIRED)

add_executable(Oasis src/main.cpp)
target_compile_features(Oasis PRIVATE cxx_std_17)
target_link_libraries(Oasis PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Oasis)
