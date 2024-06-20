include_directories(../include)

file(GLOB SRC "*.hpp" "*.cpp")
find_package(Vulkan REQUIRED)
add_library(RENDERING ${SRC})
target_link_libraries(RENDERING ${Vulkan_LIBRARY} glfw glm TINY_OBJ_LOADER STB_IMAGE UTILS PHYSICS)
target_include_directories(RENDERING PUBLIC ../include ../../../external/glad/include ${Vulkan_INCLUDE_DIRS})