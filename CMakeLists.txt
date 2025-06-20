cmake_minimum_required(VERSION 3.16) # FetchContent is available in 3.11+

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")

project(sine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generate compile_commands.json

# =================================================== DEPENDENCIES =================================================== #
set(RAYLIB_VERSION 5.5)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(raylib)
  endif()
endif()

add_subdirectory("thirdParty/imgui")
add_subdirectory("thirdParty/ldtk")

# =================================================== PROJECT =================================================== #
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE MY_LIB_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp")

add_library(${PROJECT_NAME} INTERFACE "${MY_LIB_SOURCES}")
target_link_libraries(${PROJECT_NAME} INTERFACE raylib imgui LDtkLoader::LDtkLoader)
target_include_directories(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include/")

add_executable(game "${MY_SOURCES}")
target_link_libraries(game PUBLIC raylib imgui sine)
target_include_directories(game PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

# copies the assets folder relative to the CMakeLists.txt file to the folder relative to the "out/build/GCC..." path
file(COPY "assets" DESTINATION "./Debug")

target_compile_definitions(game PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/")
#target_compile_definitions(game PUBLIC RESOURCES_PATH="./assets/") # Uncomment this and comment the one above when shipping !!!

# =================================================== CONFIGS =================================================== #
# Web Configurations
if (${PLATFORM} STREQUAL "Web")
    set_target_properties(game PROPERTIES SUFFIX ".html") # Tell Emscripten to build an example.html file.
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY -s GL_ENABLE_GET_PROC_ADDRESS=1")
endif()

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(game "-framework IOKit")
    target_link_libraries(game "-framework Cocoa")
    target_link_libraries(game "-framework OpenGL")
endif()