include_directories(../include ../src)

file(GLOB SRC "*.hpp" "*.cpp" "*.tpp")
find_package(Vulkan REQUIRED)
add_library(COMMON_RENDERING ${SRC})
target_link_libraries(COMMON_RENDERING ${Vulkan_LIBRARY} glfw glm TINY_OBJ_LOADER STB_IMAGE UTILS PHYSICS ENTITY assimp)
target_include_directories(COMMON_RENDERING PUBLIC ../include ../../../../external/glad/include ${Vulkan_INCLUDE_DIRS} ../../../../external/glfw-3.3.4/include)