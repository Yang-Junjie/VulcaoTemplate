include(FetchContent)

FetchContent_Declare(
    vulcao
    GIT_REPOSITORY  https://github.com/Yang-Junjie/vulcao.git
    GIT_TAG         93d9b4bb01a16c5c1eef5e03b567d4deae91e913
    GIT_SHALLOW     TRUE
    GIT_PROGRESS    TRUE
    GIT_SUBMODULES  third_party/vk-bootstrap
                    third_party/VulkanMemoryAllocator
                    third_party/spirv-reflect
    FIND_PACKAGE_ARGS
)

set(VULCAO_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(VULCAO_BUILD_DOCS  OFF CACHE BOOL "" FORCE)
set(VULCAO_INSTALL     OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(vulcao)
