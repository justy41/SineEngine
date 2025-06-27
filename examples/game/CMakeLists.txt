cmake_minimum_required(VERSION 3.16)
project(game VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generate compile_commands.json

add_subdirectory("thirdParty/sine-engine")



file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(game ${MY_SOURCES})
target_link_libraries(game PUBLIC sine)
target_include_directories(game PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# copies the assets folder relative to the CMakeLists.txt file to the folder relative to the "out/build/GCC..." path
file(COPY "assets" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Debug")

target_compile_definitions(game PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/")
#target_compile_definitions(game PUBLIC RESOURCES_PATH="./assets/") # Uncomment this and comment the one above when shipping !!!
