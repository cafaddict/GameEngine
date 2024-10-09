include_directories(../include ../src)

file(GLOB SRC "*.hpp" "*.cpp" "*.tpp")
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

if(OpenGL_FOUND)
    message(STATUS "OpenGL package found")
else()
    message(FATAL_ERROR "OpenGL package not found")
endif()

if(glfw3_FOUND)
    message(STATUS "GLFW3 package found")
else()
    message(FATAL_ERROR "GLFW3 package not found")
endif()

add_library(OPENGL_RENDERING ${SRC})
target_link_libraries(OPENGL_RENDERING ${Vulkan_LIBRARY} COMMON_RENDERING glfw glm TINY_OBJ_LOADER STB_IMAGE UTILS PHYSICS ENTITY assimp)
target_include_directories(OPENGL_RENDERING PUBLIC ../include ../../../../external/glad/include ${Vulkan_INCLUDE_DIRS} ../../../../external/glfw-3.3.4/include)