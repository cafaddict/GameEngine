# include_directories(../include)

# file(GLOB SRC "*.hpp" "*.cpp")

# file(GLOB IMGUI_HEADERS "../../../external/imgui/*.h" "../../../external/imgui/*.cpp")
# add_library(UI ${IMGUI_HEADERS} ${SRC})
# target_link_libraries(UI PUBLIC STB_IMAGE RENDERING)
# # target_include_directories(UI PUBLIC ../include ../../../external/glad/include ../../../external/imgui ../../../external)
# target_include_directories(UI PUBLIC ../include ../../../external/imgui)


include_directories(../include)
file(GLOB IMGUI_HEADERS "../../../external/imgui/*.h" "../../../external/imgui/*.cpp")
file(GLOB SRC "*.hpp" "*.cpp")
file(GLOB SRC_C "*.h")

add_library(UI ${SRC} ${IMGUI_HEADERS} ${SRC_C})
target_link_libraries(UI UTILS PHYSICS RENDERING EVENTS CORE)
target_include_directories(UI PUBLIC ../include ../../../external/imgui)