include_directories(../include)

file(GLOB SRC "*.hpp" "*.cpp")
add_library(ENTITY ${SRC})
target_link_libraries(ENTITY glm UTILS)
target_include_directories(ENTITY PUBLIC ../include ../../../external)