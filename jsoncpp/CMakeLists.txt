project(jsoncpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB JSONCPP_SRC ${SRC_DIR}/*.cpp ${SRC_DIR}/*h ${SRC_DIR}/*.ini)
add_library(jsoncpp STATIC ${JSONCPP_SRC})