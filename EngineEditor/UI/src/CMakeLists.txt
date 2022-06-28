include_directories(../include)

file(GLOB SRC "*.hpp" "*.cpp")
add_library(UI ${SRC})
target_include_directories(UI PUBLIC ../include ../../../external/glad/include ../../../external)