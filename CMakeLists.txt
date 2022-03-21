cmake_minimum_required(VERSION 2.8)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
project(nuEngine)
include_directories(include)
link_directories(lib)
file(GLOB project_file glad.c main.cpp pointlight.cpp camera.cpp cameracontrol.cpp shader.cpp texture2d.cpp texture_cube.cpp texture.cpp material.cpp mesh.cpp model.cpp deferred.cpp shadow_mapper.cpp framebuffer_object.cpp profiler.cpp callbacks.cpp object.cpp scenedesc.cpp scene.cpp uniform_buffer.cpp)
add_executable(${PROJECT_NAME} ${project_file})

# for windows
target_link_libraries(${PROJECT_NAME}
    opengl32
    glfw3
    assimp
    )

# for linux
# target_link_libraries(${PROJECT_NAME}
#     -lglfw3
#     -lGL
#     -lm
#     -lXrandr
#     -lXi
#     -lX11
#     -lXxf86vm
#     -lpthread
#     -ldl
#     -lXinerama
#     -lXcursor
#     -lassimp
#     )