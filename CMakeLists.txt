# General
cmake_minimum_required(VERSION 3.16)

project("ParallelSystems" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Add source files
file(GLOB_RECURSE SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/src/*.c
        ${CMAKE_SOURCE_DIR}/src/*.cc
        ${CMAKE_SOURCE_DIR}/src/*.cpp)

# Add header files
file(GLOB_RECURSE HEADER_FILES
        ${CMAKE_SOURCE_DIR}/src/*.h
        ${CMAKE_SOURCE_DIR}/src/*.hh
        ${CMAKE_SOURCE_DIR}/src/*.hpp)

# Define the executable
add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

# Search for additional libraries
find_package(OpenGL REQUIRED)

find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")

find_package(GLEW REQUIRED)
message(STATUS "Found GLEW in ${GLEW_INCLUDE_DIR}")

set(IMGUI_DIR thirdparty/imgui)
add_library(
        imgui STATIC
        ${IMGUI_DIR}/imconfig.h
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui.h
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/imgui_impl_glfw.h
        ${IMGUI_DIR}/imgui_impl_opengl3.cpp
        ${IMGUI_DIR}/imgui_impl_opengl3.h
        ${IMGUI_DIR}/imgui_impl_opengl3_loader.h
        ${IMGUI_DIR}/imgui_internal.h
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imstb_rectpack.h
        ${IMGUI_DIR}/imstb_textedit.h
        ${IMGUI_DIR}/imstb_truetype.h
)
target_include_directories("imgui" PRIVATE "${IMGUI_DIR}")
target_include_directories(${PROJECT_NAME} PRIVATE "${IMGUI_DIR}")
target_link_libraries(${PROJECT_NAME} "imgui" "${CMAKE_DL_LIBS}")
message(STATUS "Found ImGUI in ${IMGUI_DIR}")

set(LIBS glfw OpenGL::GL GLEW::glew imgui)

# Define the include DIRs
include_directories(
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/include"
)

# Link the libraries
target_link_libraries(${PROJECT_NAME} ${LIBS})