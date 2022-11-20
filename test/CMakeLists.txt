project(xTest)

set (CMAKE_CXX_STANDARD 17)

link_directories(../lib/SDL/x86)
link_directories(../lib/SDL_IMG/x86)
link_directories(../lib/box2d)

file(GLOB TEST_SRC 
#sdl_test.cpp
#egine_test.cpp
test.cpp
)

add_executable(EngineTest ${TEST_SRC})
target_link_libraries(EngineTest xEngine)

target_link_libraries(EngineTest debug box2d_d.lib)
target_link_libraries(EngineTest optimized box2d.lib)