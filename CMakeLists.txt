cmake_minimum_required(VERSION 3.0)
set(lib_name Vonsai)
project(${lib_name})

# TODO ....


# ------------------------------------------------------------------------- #
# GLOBAL SETTINGS
# ------------------------------------------------------------------------- #

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# ------------------------------------------------------------------------- #
# THIRD PARTY
# ------------------------------------------------------------------------- #

add_subdirectory(vendor/fmt)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glad_opengl_4.1)


# ------------------------------------------------------------------------- #
# PROJECT
# ------------------------------------------------------------------------- #

add_library(${lib_name} STATIC
    ./src/Utils/Colors.cpp
    ./src/Utils/Files.cpp
    ./src/Utils/Random.cpp
    ./src/Utils/Strings.cpp

    ./src/App.cpp
    ./src/Bindable.cpp
    ./src/Camera.cpp
    ./src/Renderable.cpp
    ./src/Shader.cpp
    ./src/Texture.cpp
    ./src/Transform.cpp
    ./src/UBO.cpp
    ./src/Window.cpp
)
target_link_libraries(${lib_name} PUBLIC glm PRIVATE fmt glfw glad)
target_include_directories(${lib_name} PUBLIC "./src" "./include" "./vendor")

# ------------------------------------------------------------------------- #
# TESTING
# ------------------------------------------------------------------------- #

# Vars
set(test_path test)
set(test_lib Catch)

# Link
add_library(${test_lib} INTERFACE)
target_link_libraries(${test_lib} INTERFACE fmt glm glfw glad ${lib_name})

# Executables
add_executable(test${lib_name} ${test_path}/test${lib_name}.cpp)
target_link_libraries(test${lib_name} PUBLIC ${test_lib})


# ------------------------------------------------------------------------- #
# TESTs ADDITION
# ------------------------------------------------------------------------- #

enable_testing()
add_test(COMMAND test${lib_name})
