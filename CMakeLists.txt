cmake_minimum_required(VERSION 3.16)
project(nurbs_surface LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_executable(nurbs_surface
    main.cpp
    Computational/nurbs.cpp
    camera.cpp
    window.cpp
    mesh.cpp
    material.cpp
    shader.cpp
    shadowMap.cpp
    Light/light.cpp
    Light/directionalLight.cpp
)
target_link_libraries(nurbs_surface PRIVATE OpenGL::GL GLEW::GLEW glfw glm::glm)

# Keep shader files beside the executable, including after shader-only edits.
add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Shaders"
        "${CMAKE_CURRENT_BINARY_DIR}/Shaders"
    VERBATIM
)
add_dependencies(nurbs_surface copy_shaders)

add_executable(nurbs_test Computational/test.cpp Computational/nurbs.cpp)
