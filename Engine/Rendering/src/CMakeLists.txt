include_directories(../include)

file(GLOB SRC "*.hpp" "*.cpp")
find_package(Vulkan REQUIRED)
add_library(RENDERING ${SRC})
target_link_libraries(RENDERING UTILS ${Vulkan_LIBRARY} glfw glm)
target_include_directories(RENDERING PUBLIC ../include ../../../external/glad/include)