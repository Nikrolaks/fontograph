cmake_minimum_required(VERSION 3.16)
project(fontograph)

set(CMAKE_CXX_STANDARD 20)

set(FONTOGRAPH_FOLDER "${PROJECT_SOURCE_DIR}")
set(CMAKE_MODULE_PATH "${FONTOGRAPH_FOLDER}/cmake/modules")

set(SOURCES
    lib/galogen/gl.cpp
    lib/galogen/gl.h

    lib/recycle/blob.cpp
    lib/recycle/blob.h
    lib/recycle/buffer.cpp
    lib/recycle/buffer.h
    lib/recycle/face.cpp
    lib/recycle/face.h
    lib/recycle/font.cpp
    lib/recycle/font.h
    lib/recycle/shape.cpp
    lib/recycle/shape.h

    lib/drawer.cpp
    lib/drawer.h
    lib/font_storage.cpp
    lib/font_storage.h
    lib/shaper.cpp
    lib/shaper.h
    lib/utils.cpp
    lib/utils.h
)

add_library(${PROJECT_NAME}_lib ${SOURCES})

target_link_libraries(${PROJECT_NAME}_lib "-framework Cocoa")
target_link_libraries(${PROJECT_NAME}_lib "-framework IOKit")
target_link_libraries(${PROJECT_NAME}_lib "-framework CoreFoundation")
find_package(glfw3 REQUIRED)
target_link_libraries(${PROJECT_NAME}_lib ${GLFW_LIBRARY})
target_include_directories(${PROJECT_NAME}_lib PUBLIC ${GLFW_INCLUDE_DIR})
target_compile_options(${PROJECT_NAME}_lib PUBLIC ${glfw3_CFLAGS_OTHER})

find_package(RapidJSON REQUIRED)
target_link_libraries(${PROJECT_NAME}_lib ${RAPIDJSON_LIBRARY})
target_include_directories(${PROJECT_NAME}_lib PUBLIC ${RAPIDJSON_INCLUDE_DIRS})
target_compile_options(${PROJECT_NAME}_lib PUBLIC ${RAPIDJSON_CFLAGS_OTHER})

find_package(Libpng REQUIRED)
target_link_libraries(${PROJECT_NAME}_lib ${LIBPNG_LIBRARY})
target_include_directories(${PROJECT_NAME}_lib PUBLIC ${LIBPNG_INCLUDE_DIRS})
target_compile_options(${PROJECT_NAME}_lib PUBLIC ${LIBPNG_CFLAGS_OTHER})

find_package(Harfbuzz)
target_link_libraries(${PROJECT_NAME}_lib ${HARFBUZZ_LIBRARY})
target_include_directories(${PROJECT_NAME}_lib PUBLIC ${HARFBUZZ_INCLUDE_DIR})
target_compile_options(${PROJECT_NAME}_lib PUBLIC ${HARFBUZZ_CFLAGS_OTHER})

find_package(Boost REQUIRED COMPONENTS program_options)
target_link_libraries(${PROJECT_NAME}_lib ${Boost_LIBRARIES})
target_include_directories(${PROJECT_NAME}_lib PUBLIC ${Boost_INCLUDE_DIR})
target_compile_options(${PROJECT_NAME}_lib PUBLIC ${Boost_CFLAGS_OTHER})



target_include_directories(${PROJECT_NAME}_lib PUBLIC ${FONTOGRAPH_FOLDER}/lib)
if(MSVC)
  target_compile_options(${PROJECT_NAME}_lib PRIVATE /W4 /WX)
else()
  target_compile_options(${PROJECT_NAME}_lib PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_source_files_properties(bin/main.cpp PROPERTIES
    SKIP_LINTING ON
)
add_executable(${PROJECT_NAME} bin/main.cpp)
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_lib)
if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

foreach(source IN LISTS SOURCES)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source}")
endforeach()
