cmake_minimum_required(VERSION 3.10)
project(hotel_reservation_system C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set MSYS2 paths (adjust if your installation is different)
set(MSYS2_PREFIX "C:/msys64/ucrt64")
set(CMAKE_PREFIX_PATH ${MSYS2_PREFIX} ${CMAKE_PREFIX_PATH})

# Find required packages
find_package(Curses REQUIRED)

# On MSYS2/MinGW, FindCurses can pick a static ncurses archive that doesn't match
# the headers' dllimport annotations. Force the DLL import library to avoid
# undefined references like __imp_wattr_off / __imp_init_pair.
if(WIN32)
    set(CURSES_LIBRARIES "${MSYS2_PREFIX}/lib/libncursesw.dll.a")
endif()

# Add include directories
include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/include
    ${CURSES_INCLUDE_DIRS}
    ${MSYS2_PREFIX}/include/ncurses
    ${MSYS2_PREFIX}/include
)

# Strictly compile ONLY .c files from src/ (diagramms/ and other folders are excluded)
file(GLOB SOURCES CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/src/*.c"
)

# Add main executable (using menu.c as the entry point)
add_executable(hotel_app
    ${SOURCES}
)

# Link libraries
target_link_libraries(hotel_app
    PRIVATE
    ${CURSES_LIBRARIES}
)

# Make tests optional
option(ENABLE_TESTS "Build tests" OFF)
if(ENABLE_TESTS)
    find_package(Criterion REQUIRED)
    add_subdirectory(tests)
endif()

# Print configuration summary
message(STATUS "Curses include dir: ${CURSES_INCLUDE_DIRS}")
message(STATUS "Curses libraries: ${CURSES_LIBRARIES}")
message(STATUS "MSYS2 prefix: ${MSYS2_PREFIX}")
message(STATUS "Build tests: ${ENABLE_TESTS}")
