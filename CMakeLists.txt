cmake_minimum_required(VERSION 3.28)
project(raylib+project)

include(FetchContent)
find_package(OpenGL REQUIRED)

set(WITH_PIC ON)
set(GLFW_BUILD_X11 OFF)
set(GLFW_BUILD_WAYLAND ON)
FetchContent_Declare(
  raylib
  URL      https://github.com/raysan5/raylib/archive/refs/tags/6.0.tar.gz
  URL_HASH SHA256=2b3ee1e2120c7a0796b33062c7e9a694dd8a8caa56a96319ac8c8ecf54a90d0b
)

FetchContent_MakeAvailable(raylib)

add_executable(execske_zsigukanak main.cpp)
target_link_libraries(execske_zsigukanak PUBLIC raylib)