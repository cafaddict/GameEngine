file(GLOB SRC "*.hpp" "*.cpp")
add_library(EVENTS ${SRC})
target_include_directories(EVENTS PUBLIC ../include)