add_library(libbmgr STATIC bmgr.cpp dsmgr.cpp)

target_include_directories(libbmgr PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
