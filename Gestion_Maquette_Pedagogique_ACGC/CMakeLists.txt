cmake_minimum_required(VERSION 4.2)
project(Gestion_Maquette_Pedagogique_ACGC C)

set(CMAKE_C_STANDARD 23)

add_executable(Gestion_Maquette_Pedagogique_ACGC main.c
        Maquette.c
        Maquette.h
        types.h)
