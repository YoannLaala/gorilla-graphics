cmake_minimum_required(VERSION 3.11)
include(FetchContent)

set(CMAKE_CXX_STANDARD 11)

FetchContent_Declare(
    gorilla-core
    GIT_REPOSITORY  https://github.com/YoannLaala/gorilla-core.git
    GIT_TAG         1.2.7
    SOURCE_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/.externals/src/gorilla-core
    SUBBUILD_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/.externals/temp/gorilla-core
)

if(NOT gorilla-core_POPULATED)
    FetchContent_Populate(gorilla-core)
    execute_process(COMMAND ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR} -S ${gorilla-core_SOURCE_DIR} -B ${gorilla-core_BINARY_DIR} -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX})
    execute_process(COMMAND ${CMAKE_COMMAND} --build ${gorilla-core_BINARY_DIR} --target INSTALL --config ${CMAKE_BUILD_TYPE})
endif()

project(gorilla-graphics)
set(LIBRARY_NAME ${CMAKE_STATIC_LIBRARY_PREFIX}${PROJECT_NAME})

find_package(PkgConfig)
pkg_check_modules(LIBGORILLA-CORE libgorilla-core REQUIRED)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    add_compile_definitions(PLATFORM_WINDOWS)
    set(PLATFORM_WINDOWS ON)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    add_compile_definitions(PLATFORM_LINUX)
    set(PLATFORM_LINUX ON)
elseif(UNIX AND APPLE)
    add_compile_definitions(PLATFORM_MAC)
    set(PLATFORM_MAC ON)
endif ()

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/color_rgba.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/command_list.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/command_queue.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device_info.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/device.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/enums.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/errors.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/resources/command_allocator.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/resources/descriptor_heap.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/resources/fence.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/resources/resource.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/resources/swapchain.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/resources/texture_2d.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/window.hpp
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/color_rgba.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/resources/command_allocator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/resources/descriptor_heap.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/resources/fence.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/resources/resource.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/resources/swapchain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/resources/texture_2d.cpp
)

if (PLATFORM_WINDOWS)
    set(SOURCES_PLATFORM
        ${CMAKE_CURRENT_SOURCE_DIR}/src/command_list_d3d12.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/command_queue_d3d12.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/enums_d3d12.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/device_d3d12.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/window_windows.cpp
    )
elseif (PLATFORM_MAC OR PLATFORM_LINUX)
endif()
set(SOURCES ${SOURCES} ${SOURCES_PLATFORM})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} ${HEADERS})

add_library(${LIBRARY_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${LIBRARY_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${LIBGORILLA-CORE_INCLUDE_DIRS}
)
target_link_directories(${LIBRARY_NAME} PUBLIC
    ${LIBGORILLA-CORE_LIBRARY_DIRS}
)
target_link_libraries(${LIBRARY_NAME} PUBLIC
    ${LIBGORILLA-CORE_LIBRARIES}
)

set(PC_FILENAME lib${PROJECT_NAME}.pc)
set(PC_LINK_FLAGS -l${PROJECT_NAME})
configure_file(${PC_FILENAME}.in ${PC_FILENAME} @ONLY)

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/
    DESTINATION include/${PROJECT_NAME}
    FILES_MATCHING
    PATTERN
    "*.hpp"
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PC_FILENAME} DESTINATION lib/pkgconfig)

enable_testing()
add_executable (unit-tests EXCLUDE_FROM_ALL tests/unit-tests.cpp)
target_include_directories(unit-tests PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${LIBGORILLA-CORE_INCLUDE_DIRS}
)
target_link_directories(unit-tests PUBLIC
    ${LIBGORILLA-CORE_LIBRARY_DIRS}
)
target_link_libraries(unit-tests PUBLIC
    ${LIBRARY_NAME}
    ${LIBRARIES_PLATFORM}
    ${LIBGORILLA-CORE_LIBRARIES}
    D3D12.lib
    DXGI.lib
)
add_test(color               unit-tests color)
