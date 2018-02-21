cmake_minimum_required(VERSION 3.10)
project(jeu_role_4A)

set(CMAKE_CXX_STANDARD 11)

add_executable(jeu_role_4A
        Gameplay.c
        Gameplay.h
        Graphics.c
        Graphics.h
        Inventory.c
        Inventory.h
        Main.c
        Npc.c
        Npc.h)
