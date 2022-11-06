project(xEngine)

include_directories(../inc/SDL)
link_directories(../lib/SDL/x86)
link_directories(../lib/SDL_IMG/x86)

file(GLOB SRC_FILE *.*)
add_library(xEngine ${SRC_FILE})
target_link_libraries(xEngine SDL2.lib)
target_link_libraries(xEngine SDL2_image.lib) 
