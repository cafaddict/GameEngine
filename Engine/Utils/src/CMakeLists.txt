include_directories(../include)

file(GLOB SRC "*.hpp" "*.cpp")

add_library(UTILS ${SRC})
target_include_directories(UTILS PUBLIC ../include ../../../external)