add_library(Tag STATIC Tag.cpp)
target_include_directories(Tag PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Tag PUBLIC TagUtil)
target_link_libraries(Tag PUBLIC TagGroup)
