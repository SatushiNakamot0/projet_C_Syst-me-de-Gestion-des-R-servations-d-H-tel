cmake_minimum_required(VERSION 3.16)
project(hotel_management C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(nuklear INTERFACE)
target_include_directories(nuklear INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/external/nuklear)

set(SRC_FILES
    main.c
    menu.c
    clients.c
    chambres.c
    reservations.c
    facturation.c
    fichiers.c
    validation.c
    authentication.c
    notifications.c
    ui_theme.c
    sha256.c
    nuklear_impl.c
)

add_executable(hotel_management ${SRC_FILES})
target_include_directories(hotel_management PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(hotel_management PRIVATE nuklear)

