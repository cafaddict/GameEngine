project(Sandbox)

add_executable(SandBox SandBox.cpp)
target_link_libraries(SandBox PUBLIC ENGINE EDITOR)
target_include_directories(SandBox PUBLIC ../Engine ../EngineEditor)
