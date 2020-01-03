cmake_minimum_required (VERSION 3.8)

project (estun)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")


set (CMAKE_POSITION_INDEPENDENT_CODE ON)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)

include_directories(${Vulkan_INCLUDE_DIR})

set(ALL_LIBS  ${Vulkan_LIBRARY} )

# ХЗ на счет этого мож для каждого своё надо

# GLFW
add_subdirectory(estun/dependencies/glfw)
include_directories(estun/dependencies/glfw/include)


#ImGUI
add_subdirectory(estun/dependencies/imgui)
include_directories(estun/dependencies/imgui)

#STBI
add_subdirectory(estun/dependencies/stbi)
include_directories(estun/dependencies/stbi)

#TinyOBJ
add_subdirectory(estun/dependencies/tinyobjloader)
include_directories(estun/dependencies/tinyobjloader)

include_directories (estun/src)
include_directories (estun/dependencies/spdlog/include)
include_directories (estun/dependencies/glm)

message(${ALL_LIBS})

# Engine
file(GLOB_RECURSE HEADER_FILES
	estun/src/*.h
    )

file(GLOB_RECURSE SOURCE_FILES
	estun/src/*.cpp
    main.cpp
    )

#message(${SOURCE_FILES})

#uncomment this to detect broken memory problems via gcc sanitizers
#set(ASAN_OPTIONS fast_unwind_on_malloc=0)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=address -fsanitize-address-use-after-scope -fno-omit-frame-pointer -fsanitize=leak -fsanitize=undefined -fsanitize=bounds-strict")

#add_library(estun STATIC ${HEADER_FILES} ${SOURCE_FILES})
add_executable(test ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(test ${ALL_LIBS} glfw imgui stbi tinyobjloader)
