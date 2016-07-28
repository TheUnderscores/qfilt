cmake_minimum_required(VERSION 2.8.8)

project(qfilt)

set(CMAKE_C_FLAGS "-Wall -Wextra -Wshadow -Wcast-align -Wwrite-strings -Winline \
-Wno-attributes -Wno-deprecated-declarations -Wno-div-by-zero -Wno-endif-labels \
-Wfloat-equal -Wformat=2 -Wno-format-extra-args -Winit-self -Winvalid-pch \
-Wmissing-format-attribute -Wmissing-include-dirs -Wno-multichar -Wshadow \
-Wno-sign-compare -Wswitch -Wno-pragmas -Wno-unused-but-set-parameter \
-Wno-unused-but-set-variable -Wno-unused-result -Wwrite-strings \
-Wdisabled-optimization -Wpointer-arith -Wconversion")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Og")

include_directories("include/")

file(GLOB qfilt_src
  "src/*.c")

add_executable(qfilt ${qfilt_src})

target_link_libraries(qfilt -lm)

find_package(PNG)

if (PNG_FOUND)
  if (${PNG_VERSION_STRING} VERSION_LESS "1.6")
    message(FATAL_ERROR "Need a 1.6.x series libpng; \
you have ${PNG_VERSION_STRING}")
  endif (${PNG_VERSION_STRING} VERSION_LESS "1.6")

  include_directories(${PNG_INCLUDE_DIRS})
  target_link_libraries(qfilt ${PNG_LIBRARIES})
endif (PNG_FOUND)
