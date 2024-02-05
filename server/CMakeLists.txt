cmake_minimum_required(VERSION 3.17)
project(upscloveceserver)

set(CMAKE_CXX_STANDARD 17)

add_executable(upscloveceserver main.cpp server.h server.cpp type.h message.cpp message.h game.cpp game.h
        player.cpp player.h)
