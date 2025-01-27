cmake_minimum_required(VERSION 3.13)
project("tib2farmer" VERSION 0.1.0 LANGUAGES C)
set(CMAKE_C_STANDARD 23)

# set(CMAKE_BUILD_TYPE Release CACHE STRING "Release or Debug" FORCE)
# set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")

# if(POLICY CMP0141)
#   cmake_policy(SET CMP0141 NEW)
#   set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
# endif()

# find_library(PDCurses_LIBRARY NAMES pdcurses PATHS "C:/msys64/mingw64/lib")
# include_directories("C:/msys64/mingw64/include")
find_library(PDCurses_LIBRARY NAMES pdcurses PATHS "${PROJECT_SOURCE_DIR}/lib")
include_directories("${PROJECT_SOURCE_DIR}/include")

add_compile_options(-Wall -Wextra -pedantic -Wunused-variable)
add_compile_options(-Wno-format)

set(SOURCE_FILES "src/tib2farmer.c")

add_executable(tib2farmer ${SOURCE_FILES})
target_link_libraries(tib2farmer ${PDCurses_LIBRARY})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(tib2farmer PRIVATE -g -O0)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  target_compile_options(tib2farmer PRIVATE -O2 -march=native -s -static)
endif()

set_property(TARGET tib2farmer PROPERTY C_STANDARD 23)
