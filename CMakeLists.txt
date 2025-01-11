cmake_minimum_required(VERSION 3.10)

project(
    compute_shaders 
    VERSION 1.0
)

include("$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/Application.cpp
    src/ApplicationMinimal.cpp
    src/CameraController.cpp
    src/GUI.cpp

    src/Util/Keyboard.cpp
    src/Util/Maths.cpp
    src/Util/Profiler.cpp
    src/Util/Util.cpp

    src/Graphics/Camera.cpp
    src/Graphics/Mesh.cpp

    src/Graphics/OpenGL/Framebuffer.cpp
    src/Graphics/OpenGL/GLUtils.cpp
    src/Graphics/OpenGL/Shader.cpp
    src/Graphics/OpenGL/Texture.cpp
    src/Graphics/OpenGL/VertexArray.cpp
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)

target_compile_definitions(${PROJECT_NAME} PRIVATE GLM_ENABLE_EXPERIMENTAL)

if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "/O2")
    set(CMAKE_CXX_FLAGS_RELEASE "/Ox")
  	target_compile_options(${PROJECT_NAME} PRIVATE 
    	/W4)
else()
  	target_compile_options(${PROJECT_NAME} PRIVATE 
		-Wall -Wextra -pedantic)		
endif()

find_package(glm CONFIG REQUIRED)
find_package(SFML COMPONENTS system audio network window graphics CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

add_subdirectory(deps)
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    deps
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    sfml-system sfml-audio sfml-network sfml-graphics sfml-window
    imgui::imgui
    glm::glm
    imgui_sfml
    glad 
)
