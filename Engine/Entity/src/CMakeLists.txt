include_directories(../include)

file(GLOB SRC "*.hpp" "*.cpp")
add_library(ENTITY ${SRC})
target_link_libraries(ENTITY glm UTILS TINY_OBJ_LOADER STB_IMAGE)
target_include_directories(ENTITY PUBLIC ../include ../../../external)