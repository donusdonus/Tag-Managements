set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- ดึง fmt ด้วย FetchContent ---
include(FetchContent)
FetchContent_Declare(
  LinkList
  GIT_REPOSITORY https://github.com/donusdonus/LinkList.git
  GIT_TAG main
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(LinkList)  # สร้าง target fmt::fmt

# เลือกชื่อ target ที่มีอยู่จริงจาก repo (กันพังเรื่องชื่อ)
if(TARGET LinkList::LinkList)
  set(_LinkListTarget LinkList::LinkList)
elseif(TARGET LinkList)
  set(_LinkListTarget LinkList)
else()
  message(FATAL_ERROR "ไม่พบ target จาก repo LinkList (คาดว่า 'LinkList' หรือ 'LinkList::LinkList')")
endif()


add_library(TagGroup STATIC TagGroup.cpp)
target_include_directories(TagGroup PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# ถ้า 'Tag' เป็นอีก target หนึ่งอยู่แล้ว อันนี้โอเค
target_link_libraries(TagGroup PUBLIC Tag)

# ลิงก์กับ LinkList ที่ดึงมาจริง
target_link_libraries(TagGroup PUBLIC ${_LinkListTarget})
