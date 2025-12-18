cmake_minimum_required(VERSION 3.13)
project(tib2farmer VERSION 0.3.0 LANGUAGES C)

set(CMAKE_C_STANDARD 23)

add_compile_options(-Wall -Wextra -pedantic -Wunused-variable)
add_compile_options(-Wno-format)

include_directories("${PROJECT_SOURCE_DIR}/include")
# set(WIN32_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources/resources.rc)

set(SOURCE_FILES
    src/main.c
    src/game_data.c
    src/data_retrieval.c
    src/timer.c
    src/ui.c
    src/utils.c
)

set(HEADER_FILES
    src/game_data.h
    src/data_retrieval.h
    src/timer.h
    src/ui.h
    src/utils.h
    src/pdc_config.h
)

add_executable(tib2farmer
    ${SOURCE_FILES}
    ${HEADER_FILES}
    # ${WIN32_RESOURCES}
)

target_link_libraries(tib2farmer
    "${PROJECT_SOURCE_DIR}/lib/pdcurses.a"
    winmm
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(tib2farmer PRIVATE -g3 -O0)
    target_link_options(tib2farmer PRIVATE -static-libgcc -static-libstdc++ -static)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(tib2farmer PRIVATE -O2 -march=native -s)
    target_link_options(tib2farmer PRIVATE -static-libgcc -static-libstdc++ -static)
endif()

set_property(TARGET tib2farmer PROPERTY C_STANDARD 23)
