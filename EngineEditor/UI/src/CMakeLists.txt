include_directories(../include)

file(GLOB SRC "*.hpp" "*.cpp")

file(GLOB IMGUI_HEADERS "../../../external/imgui/*.h" "../../../external/imgui/*.cpp")
add_library(UI ${SRC} ${IMGUI_HEADERS})
target_link_libraries(UI PUBLIC ENGINE)
target_include_directories(UI PUBLIC ../include ../../../external/glad/include ../../../external/imgui ../../../external)