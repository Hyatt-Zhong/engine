project(xTest)

link_directories(../lib/SDL/x86)
link_directories(../lib/SDL_IMG/x86)

file(GLOB TEST_SRC 
#sdl_test.cpp
#egine_test.cpp
test.cpp
)

add_executable(EngineTest ${TEST_SRC})
target_link_libraries(EngineTest xEngine)