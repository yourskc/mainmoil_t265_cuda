#  minimum required cmake version: 3.1.0
cmake_minimum_required(VERSION 3.1.0)
#set_property(GLOBAL PROPERTY USE_FOLDERS ON)
project(MoilRealsenseExample)

set(LZ4_DIR lz4)
find_package(OpenCV REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(GLUT REQUIRED)
find_package(CUDA REQUIRED)
include_directories( include ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${LZ4_DIR})
link_directories(${CMAKE_SOURCE_DIR}/lib)
set(DEPENDENCIES libmoildev.a ${OpenCV_LIBS} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} realsense2 glfw cuda cudart)
add_subdirectory(glfw)
add_executable(mainmoil main.cpp moilview.cpp moilcuda.o)
target_compile_features(mainmoil PUBLIC cxx_std_11)
set_property(TARGET mainmoil PROPERTY CXX_STANDARD 11)
target_link_libraries(mainmoil ${DEPENDENCIES})

install(
	TARGETS mainmoil RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)
