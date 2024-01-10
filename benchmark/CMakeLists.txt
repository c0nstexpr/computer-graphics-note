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

set_target_properties(${PROJECT_NAME}Lib PROPERTIES COMPILE_WARNING_AS_ERROR ON)

target_compile_options(
  ${PROJECT_NAME}Lib
  INTERFACE $<$<CXX_COMPILER_ID:MSVC>:/W4>
            $<$<CXX_COMPILER_ID:Clang>:-Qunused-arguments -ftime-trace
            -fdiagnostics-show-template-tree>)

config_exe(${PROJECT_NAME} EXE_SRC ${src})

target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}Lib)

#
# Configure clang-tidy
#
option(ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)

if(${ENABLE_CLANG_TIDY})
  message(STATUS "clang-tidy is enabled")

  target_enable_clang_tidy(${PROJECT_NAME})
endif()
