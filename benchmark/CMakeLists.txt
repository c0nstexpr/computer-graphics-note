#
# Project details
#
project(${CMAKE_PROJECT_NAME}Benchmark LANGUAGES CXX)

verbose_message("Adding benchmark under ${CMAKE_PROJECT_NAME}...")

find_package(benchmark CONFIG REQUIRED)

#
# Set the sources for the unit tests and add the executable(s)
#
set(src src/rasterization/raster_triangle.cpp)

config_lib(${PROJECT_NAME}Lib INTERFACE)

target_link_libraries(
  ${PROJECT_NAME}Lib
  INTERFACE ${CMAKE_PROJECT_NAME} benchmark::benchmark
            ${CMAKE_PROJECT_NAME}TestsLib benchmark::benchmark_main)

config_exe(${PROJECT_NAME} EXE_SRC ${src})

target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}Lib)
