cmake_minimum_required(VERSION 3.25)
project(soft_body LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- raylib ------------------------------------------------------------------
# Compute shadery i SSBO wymagają OpenGL 4.3 (domyślnie raylib buduje się pod 3.3).
set(OPENGL_VERSION "4.3" CACHE STRING "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.5
    GIT_SHALLOW    TRUE
    SYSTEM
)
FetchContent_MakeAvailable(raylib)

# --- aplikacja ---------------------------------------------------------------
add_executable(soft_body
    src/main.cpp
    src/blob.cpp
    src/debug_draw.cpp
)
target_link_libraries(soft_body PRIVATE raylib)

# Shadery są czytane bezpośrednio z folderu źródeł, więc po edycji pliku
# nie trzeba nic kopiować (przydatne do przeładowywania w locie).
target_compile_definitions(soft_body PRIVATE SHADER_DIR="${CMAKE_SOURCE_DIR}/shaders/")

if(MSVC)
    target_compile_options(soft_body PRIVATE /W4 /utf-8)
else()
    target_compile_options(soft_body PRIVATE -Wall -Wextra)
endif()
