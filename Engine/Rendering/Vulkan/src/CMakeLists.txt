include_directories(../include ../src)

file(GLOB SRC "*.hpp" "*.cpp" "*.tpp")
find_package(Vulkan REQUIRED)
add_library(VULKAN_RENDERING ${SRC})
target_link_libraries(VULKAN_RENDERING ${Vulkan_LIBRARY} COMMON_RENDERING glfw glm TINY_OBJ_LOADER STB_IMAGE UTILS PHYSICS ENTITY assimp)
target_include_directories(VULKAN_RENDERING PUBLIC ../include ../../../external/glad/include ${Vulkan_INCLUDE_DIRS} ../../../external/glfw-3.3.4/include)