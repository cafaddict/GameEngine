include_directories(../include)

file(GLOB SRC "*.hpp" "*.cpp")
add_library(RENDERING ${SRC})
target_include_directories(RENDERING PUBLIC ../include ../../../external/glad/include)