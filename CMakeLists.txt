cmake_minimum_required(VERSION 3.10)
project(HotelReservationSystem C)

# Enforce C99 standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Auto-discover all source files from src/ and ui/ directories
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/ui/*.c"
)

# Exclude problematic files if they exist
list(FILTER PROJECT_SOURCES EXCLUDE REGEX ".*ui_sources\\.c$")

# Auto-discover all header files from include/ directory
file(GLOB_RECURSE PROJECT_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

# Create executable with auto-discovered sources
add_executable(hotel_app ${PROJECT_SOURCES} ${PROJECT_HEADERS})

# Dynamic library handling for cross-platform compatibility
if(NOT WIN32)
    find_package(Curses REQUIRED)
endif()

# Include directories - relative paths only
target_include_directories(hotel_app
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        $<$<NOT:$<PLATFORM_ID:Windows>>:${CURSES_INCLUDE_DIRS}>
)

# Platform-specific library linking
if(WIN32)
    # Windows: Auto-detect MSYS2/MinGW paths dynamically
    find_program(MSYS2_PREFIX NAMES ucrt64 mingw64 PATHS
        "C:/msys64/ucrt64"
        "C:/msys64/mingw64"
        "C:/tools/msys64/ucrt64"
        DOC "MSYS2 installation prefix"
    )
    
    if(MSYS2_PREFIX)
        message(STATUS "Found MSYS2 at: ${MSYS2_PREFIX}")
        target_include_directories(hotel_app PRIVATE "${MSYS2_PREFIX}/include/ncurses")
        
        # Find the ncurses library dynamically
        find_library(NCURSES_LIB
            NAMES ncursesw libncursesw ncurses
            PATHS "${MSYS2_PREFIX}/lib"
            DOC "NCurses library"
        )
        
        if(NCURSES_LIB)
            target_link_libraries(hotel_app PRIVATE ${NCURSES_LIB})
        else()
            message(FATAL_ERROR "NCurses library not found in ${MSYS2_PREFIX}/lib")
        endif()
    else()
        # Fallback to standard ncurses library
        target_include_directories(hotel_app PRIVATE /mingw64/include /mingw64/include/ncurses)
        target_link_libraries(hotel_app PRIVATE ncurses)
    endif()
else()
    # Linux/Unix: Use standard Curses library
    target_link_libraries(hotel_app PRIVATE ${CURSES_LIBRARIES})
endif()

# Compiler-specific optimizations
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_compile_options(hotel_app PRIVATE -Wall -Wextra -O2)
elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_compile_options(hotel_app PRIVATE -Wall -Wextra -O2)
elseif(MSVC)
    target_compile_definitions(hotel_app PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# Optional tests
option(ENABLE_TESTS "Build tests" OFF)
if(ENABLE_TESTS)
    find_package(Criterion REQUIRED)
    add_subdirectory(tests)
endif()

# Configuration summary
message(STATUS "=== Hotel Reservation System Build Configuration ===")
message(STATUS "C Standard: ${CMAKE_C_STANDARD}")
message(STATUS "Curses Include: ${CURSES_INCLUDE_DIRS}")
message(STATUS "Curses Libraries: ${CURSES_LIBRARIES}")
message(STATUS "Source Files: ${PROJECT_SOURCES}")
message(STATUS "Header Files: ${PROJECT_HEADERS}")
if(WIN32 AND MSYS2_PREFIX)
    message(STATUS "MSYS2 Prefix: ${MSYS2_PREFIX}")
endif()
message(STATUS "Build Tests: ${ENABLE_TESTS}")
message(STATUS "===================================================")
