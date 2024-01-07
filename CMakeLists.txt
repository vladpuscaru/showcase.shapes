cmake_minimum_required(VERSION 3.14)
project(COMP4300_Assignment2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES src/main.cpp
        src/Components/Components.hpp
        src/Entity/Entity.cpp
        src/Entity/Entity.h
        src/EntityManager/EntityManager.cpp
        src/EntityManager/EntityManager.h
        src/GameEngine/GameEngine.cpp
        src/GameEngine/GameEngine.h
        src/ConfigSpecification.h
        ../libs/imgui-master/imgui.cpp
        ../libs/imgui-master/imgui_widgets.cpp
        ../libs/imgui-master/imgui_draw.cpp
        ../libs/imgui-master/imgui_tables.cpp
        ../libs/imgui-master/imgui-SFML.cpp)
add_executable(COMP4300_Assignment2 ${SOURCE_FILES})
include_directories(/usr/local/include ../libs ../libs/imgui-master)

find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories(${SFML_INCLUDE_DIRS})
target_link_libraries(COMP4300_Assignment2 sfml-system sfml-window sfml-graphics sfml-audio sfml-network ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})

file(COPY resources DESTINATION .)