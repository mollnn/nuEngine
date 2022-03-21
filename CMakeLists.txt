cmake_minimum_required(VERSION 2.8)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
project(nuEngine)
include_directories(include)
link_directories(lib)
aux_source_directory(${PROJECT_SOURCE_DIR}/src SRCSRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/ext SRCEXT)
add_executable(${PROJECT_NAME} ${SRCSRC} ${SRCEXT})

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