cmake_minimum_required(VERSION 3.10)
project(NodeEditor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ========================
# OpenCV
# ========================
# Set path to OpenCV config
set(OpenCV_DIR "C:/Users/PARTH/Downloads/opencv/build")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# ========================
# GLFW
# ========================
add_subdirectory(libs/glfw)  # Adds GLFW source directory
include_directories(libs/glfw/include)

# ========================
# GLAD
# ========================
include_directories(include)  # This should point to folder containing glad.h and KHR/
set(GLAD_SRC src/glad.c)

# ========================
# ImGui
# ========================
file(GLOB IMGUI_SRC
    libs/imgui/*.cpp
    libs/imgui/backends/imgui_impl_glfw.cpp
    libs/imgui/backends/imgui_impl_opengl3.cpp
)
include_directories(libs/imgui)
include_directories(libs/imgui/backends)

# ========================
# Add Node Files
# ========================
file(GLOB NODE_SRC
    nodes/BlurNode.cpp  # Ensure BlurNode is included here
    nodes/ThresholdNode.cpp
    nodes/EdgeDetectionNode.cpp
)

# ========================
# Executable
# ========================
add_executable(${PROJECT_NAME}
    src/main.cpp
    ${GLAD_SRC}
    ${IMGUI_SRC}
    ${NODE_SRC}  # Add the nodes source files
)

# ========================
# Link libraries
# ========================
target_link_libraries(${PROJECT_NAME}
    glfw            # Linked from add_subdirectory
    ${OpenCV_LIBS}  # Linked OpenCV
    opengl32        # Windows OpenGL system library
)
