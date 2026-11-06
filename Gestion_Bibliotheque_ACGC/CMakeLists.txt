cmake_minimum_required(VERSION 4.2)
project(Bibliotheque C)

set(CMAKE_C_STANDARD 23)

add_executable(Bibliotheque main.c
        Ouvrage.c
        Ouvrage.h
        types.h)
