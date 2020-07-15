# CMake generated Testfile for 
# Source directory: /Users/cambalamas/dev/vonsai
# Build directory: /Users/cambalamas/dev/vonsai/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(COMMAND "VonsaiSandbox")
set_tests_properties(COMMAND PROPERTIES  _BACKTRACE_TRIPLES "/Users/cambalamas/dev/vonsai/CMakeLists.txt;110;add_test;/Users/cambalamas/dev/vonsai/CMakeLists.txt;0;")
subdirs("vendor/assimp")
subdirs("vendor/fmt")
subdirs("vendor/glm")
subdirs("vendor/glfw")
subdirs("vendor/glad_opengl_4.1")
