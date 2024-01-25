cmake_minimum_required(VERSION 3.24)

#
# Project details
#
project(
  graphics
  VERSION 0.0.1
  LANGUAGES CXX)

#
# Model project dependencies
#
find_package(stdsharp CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

include(stdsharp/Utils)

config_lib(${PROJECT_NAME} INTERFACE STD 23)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} INTERFACE stdsharp::stdsharp glm::glm)
target_compile_definitions(${PROJECT_NAME} INTERFACE GLM_FORCE_UNRESTRICTED_GENTYPE)

verbose_message("Successfully added all dependencies and linked against them.")

include(CTest)
add_subdirectory(tests)
add_subdirectory(benchmark)
