cmake_minimum_required(VERSION 3.21)
project(untitled C)

set(CMAKE_C_STANDARD 99)

add_executable(vam
        src/flt.c
        src/flt_large.c
        src/bmd.c
        src/extrem_large.c
        src/allocator.c
        src/oh.c
        src/page_discard.c
        main.c )

