cmake_minimum_required(VERSION 3.10)
project(TheCommemorationOfWhiteLight VERSION 0.1.0)

set(JSON_INCLUDE_DIR ../json/include)
include_directories(${JSON_INCLUDE_DIR})


set(SDL2_INCLUDE_DIR ../SDL2/include)
set(SDL2_LIB_DIR ../SDL2/lib)
include_directories(${SDL2_INCLUDE_DIR})
link_directories(${SDL2_LIB_DIR})

set(LUA_INCLUDE_DIR ../lua/include)
set(LUA_LIB_DIR ../lua/lib)
include_directories(${LUA_INCLUDE_DIR})
link_directories(${LUA_LIB_DIR})


SET(RESOURCE_FILE resource.res)

file(GLOB source_files
            "src/*.cpp"
            "src/*.h"
            "src/*/*.cpp"
            "src/*/*.h"
    )

add_executable(TheCommemorationOfWhiteLight ${source_files})
set_target_properties(TheCommemorationOfWhiteLight PROPERTIES LINK_FLAGS ${RESOURCE_FILE} )

target_link_libraries( TheCommemorationOfWhiteLight mingw32 SDL2main SDL2 SDL2_net)
target_link_libraries( TheCommemorationOfWhiteLight -static-libgcc -static-libstdc++)
target_link_libraries( TheCommemorationOfWhiteLight lua53)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
