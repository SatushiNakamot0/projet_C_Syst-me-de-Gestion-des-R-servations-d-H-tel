cmake_minimum_required(VERSION 3.10)
project(HotelReservationSystem C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Windows/MSYS2: help CMake find ncurses (FindCurses doesn't always locate it with MinGW)
if(WIN32)
    set(MSYS2_PREFIX "C:/msys64/ucrt64" CACHE PATH "MSYS2 UCRT64 prefix")
    list(PREPEND CMAKE_PREFIX_PATH "${MSYS2_PREFIX}")

    if(NOT DEFINED CURSES_INCLUDE_PATH)
        set(CURSES_INCLUDE_PATH "${MSYS2_PREFIX}/include" CACHE PATH "Curses include path")
    endif()
    if(NOT DEFINED CURSES_LIBRARY)
        set(CURSES_LIBRARY "${MSYS2_PREFIX}/lib/libncursesw.dll.a" CACHE FILEPATH "Curses library")
    endif()
endif()

# Find required packages
find_package(Curses REQUIRED)

# Sources: ONLY from src/ (diagramms/ stays in repo but is never compiled)
file(GLOB PROJECT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

# Exclude ui_sources.c as it includes UI files directly
list(FILTER PROJECT_SOURCES EXCLUDE REGEX ".*ui_sources\\.c$")

# UI sources
file(GLOB UI_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/ui/*.c"
)

add_executable(hotel_app ${PROJECT_SOURCES} ${UI_SOURCES})

target_include_directories(hotel_app
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CURSES_INCLUDE_DIRS}
)

if(WIN32)
    target_include_directories(hotel_app
        PRIVATE
            "${MSYS2_PREFIX}/include/ncurses"
    )
endif()

# On MSYS2/MinGW, FindCurses can pick a static ncurses archive that doesn't match
# the headers' dllimport annotations. Force the DLL import library to avoid
# undefined references like __imp_wattr_off / __imp_init_pair.
if(WIN32)
    target_link_libraries(hotel_app PRIVATE "C:/msys64/ucrt64/lib/libncursesw.dll.a")
else()
    target_link_libraries(hotel_app PRIVATE ${CURSES_LIBRARIES})
endif()

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
