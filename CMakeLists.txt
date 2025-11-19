cmake_minimum_required(VERSION 4.0)
project(Physics-projects LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "Restricted build types" FORCE)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_directory_properties(PROPERTIES VS_SOLUTION_ITEMS "CMakeLists.txt;LICENSE.md;README.md;.gitignore")
source_group("Documentation" FILES CMakeLists.txt LICENSE.md README.md .gitignore)

add_subdirectory(Differential-eq)
add_subdirectory(Fourier-transform)
add_subdirectory(Mandelbrot-set)
add_subdirectory(Wind-tunnel)

set(PATH_SFML "C:/SFML-3.0.0" CACHE STRING "Path to the SFML library")