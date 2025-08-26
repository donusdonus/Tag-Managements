add_library(TagGroup STATIC TagGroup.cpp)
target_include_directories(TagGroup PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(TagGroup PUBLIC Tag)