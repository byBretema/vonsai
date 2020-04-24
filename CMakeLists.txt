cmake_minimum_required(VERSION 3.0)
set(lib_name Vonsai)
project(${lib_name})

# ------------------------------------------------------------------------- #
# THIRD PARTY
# ------------------------------------------------------------------------- #

# Disable assimp extras
set (ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set (ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set (ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
# Disable assimp exports
set (ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
# Enable specific assimp imports
set (ASSIMP_BUILD_FBX_IMPORTER ON BOOL)
set (ASSIMP_BUILD_OBJ_IMPORTER ON BOOL)
set (ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)

add_subdirectory(vendor/assimp)
add_subdirectory(vendor/fmt)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glad_opengl_4.1)

# ------------------------------------------------------------------------- #
# GLOBAL SETTINGS
# ------------------------------------------------------------------------- #

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -ggdb -Wall -Wpedantic -Wextra -Wweak-vtables -fexceptions")
# if(VONSAI_SANITIZE_THREAD)
# set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -ggdb -Wall -Wpedantic -Wextra -Wweak-vtables -fexceptions -fsanitize=address -fsanitize=undefined -fsanitize=dataflow -fsanitize=safe-stack")
# set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -ggdb -Wall -Wpedantic -Wextra -Wweak-vtables -fexceptions -fsanitize=thread -fsanitize=undefined -fsanitize=dataflow -fsanitize=safe-stack")
# set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -ggdb -Wall -Wpedantic -Wextra -Wweak-vtables -fexceptions -fsanitize=memory -fsanitize=undefined -fsanitize=dataflow -fsanitize=safe-stack")

# ------------------------------------------------------------------------- #
# PROJECT
# ------------------------------------------------------------------------- #

option(VONSAI_HIDE_MSGS "Hide logger messagges" OFF)
option(VONSAI_HIDE_DEBUG_MSGS "Hide debug messagges" OFF)

if(VONSAI_HIDE_MSGS)
    add_definitions(-DVONSAI_HIDE_MSGS)
endif()
if(VONSAI_HIDE_DEBUG_MSGS)
    add_definitions(-DVONSAI_HIDE_DEBUG_MSGS)
endif()

add_library(${lib_name} STATIC
    ./vendor/imgui/imgui.cpp
    ./vendor/imgui/imgui_demo.cpp
    ./vendor/imgui/imgui_draw.cpp
    ./vendor/imgui/imgui_widgets.cpp
    ./vendor/imgui/imgui_impl_glfw.cpp
    ./vendor/imgui/imgui_impl_opengl3.cpp

    ./src/IO/Input.cpp
    ./src/IO/Window.cpp

    ./src/Utils/Colors.cpp
    ./src/Utils/Files.cpp
    ./src/Utils/Random.cpp
    ./src/Utils/Strings.cpp
    ./src/Utils/Timer.cpp

    ./src/Bindable.cpp
    ./src/Camera.cpp
    ./src/Engine.cpp
    ./src/MeshLoader.cpp
    ./src/Renderable.cpp
    ./src/Scene.cpp
    ./src/Shader.cpp
    ./src/Texture.cpp
    ./src/Transform.cpp
    ./src/UBO.cpp
)
target_link_libraries(${lib_name} PRIVATE assimp fmt glad glm glfw)
target_include_directories(${lib_name} PUBLIC "./src" "./include" "./vendor" "./vendor/assimp/include")

# ------------------------------------------------------------------------- #
# TESTING
# ------------------------------------------------------------------------- #

# Vars
set(test_path test)
# set(test_lib Catch)

# Link
# add_library(${test_lib} INTERFACE)
# target_link_libraries(${test_lib} INTERFACE assimp fmt glad glm glfw ${lib_name})

# Executables
add_executable(${lib_name}Sandbox ${test_path}/${lib_name}Sandbox.cpp)
target_link_libraries(${lib_name}Sandbox PRIVATE assimp fmt glad glm glfw ${lib_name})


# ------------------------------------------------------------------------- #
# TESTs ADDITION
# ------------------------------------------------------------------------- #

enable_testing()
add_test(COMMAND ${lib_name}Sandbox)
