include_directories(../include)

file(GLOB SRC "*.hpp" "*.cpp")
add_library(CORE ${SRC})
target_link_libraries(CORE glfw EVENTS)
target_include_directories(CORE PUBLIC ../include ../../../external)

# target_compile_options(CORE PRIVATE -Wall -Wextra -Wpedantic -Werror)