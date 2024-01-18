#
# Project details
#
project(${CMAKE_PROJECT_NAME}Benchmark LANGUAGES CXX)

verbose_message("Adding benchmark under ${CMAKE_PROJECT_NAME}...")

find_package(nanobench CONFIG REQUIRED)
find_package(cxxopts CONFIG REQUIRED)

#
# Set the sources for the unit tests and add the executable(s)
#
set(src src/main.cpp)

config_lib(${PROJECT_NAME}Lib INTERFACE)

target_link_libraries(
  ${PROJECT_NAME}Lib INTERFACE ${CMAKE_PROJECT_NAME} nanobench::nanobench
                               cxxopts::cxxopts)

config_exe(${PROJECT_NAME} EXE_SRC ${src})

target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}Lib)
