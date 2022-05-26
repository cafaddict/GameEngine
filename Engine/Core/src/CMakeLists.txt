include_directories(../include)

file(GLOB SRC "*.hpp" "*.cpp")
add_library(CORE ${SRC})
target_include_directories(CORE PUBLIC ../include)

# target_compile_options(CORE PRIVATE -Wall -Wextra -Wpedantic -Werror)