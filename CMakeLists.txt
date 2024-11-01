cmake_minimum_required(VERSION 3.30)
project(sfml_setup)

set(SFML_STATIC_LIBRARIES TRUE)
set(BUILD_SHARED_LIBS FALSE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

file(GLOB SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE include)

# SFML
add_subdirectory(external/SFML)
target_include_directories(${PROJECT_NAME} PRIVATE external/SFML/include)
target_link_libraries(${PROJECT_NAME} PUBLIC sfml-audio sfml-graphics sfml-main sfml-network sfml-system sfml-window)

# add_custom_command( # copy dynamic libs to bin directory
#     TARGET ${PROJECT_NAME}
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#     ${CMAKE_BINARY_DIR}/external/SFML/lib 
#     ${CMAKE_BINARY_DIR}
# )