set(FETCHCONTENT_QUIET OFF CACHE BOOL "Show FetchContent download and population logs")

include(${CMAKE_CURRENT_LIST_DIR}/third_party_sdl3.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/third_party_glm.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/third_party_vulcao.cmake)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../vendor/tinyobjloader tinyobjloader)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../vendor/stb stb)
