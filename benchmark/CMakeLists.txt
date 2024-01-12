include(FetchContent)

#
# Project details
#
project(${CMAKE_PROJECT_NAME}Benchmark LANGUAGES CXX)

verbose_message("Adding benchmark under ${CMAKE_PROJECT_NAME}...")

find_package(benchmark CONFIG REQUIRED)

set(compare_py ${CMAKE_CURRENT_BINARY_DIR}/compare.py)

file(DOWNLOAD
     https://raw.githubusercontent.com/google/benchmark/main/tools/compare.py
     ${compare_py})

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

add_custom_target(
  ${PROJECT_NAME}BenchmarkReport ALL
  DEPENDS ${PROJECT_NAME}
  COMMAND ${CMAKE_COMMAND} -E ${compare_py} $<TARGET_FILE:${PROJECT_NAME}>
  USES_TERMINAL)
