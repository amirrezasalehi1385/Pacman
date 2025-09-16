cmake_minimum_required(VERSION 3.20)
project(Pacman)

set(CMAKE_CXX_STANDARD 17)

set(SDL2_DIR "C:/Users/ASUS/CLionProjects/Pacman/SDL2/x86_64-w64-mingw32")
set(SDL2_INCLUDE_DIR "${SDL2_DIR}/include")
set(SDL2_LIB_DIR "${SDL2_DIR}/lib")

set(SDL2_IMAGE_DIR "C:/Users/ASUS/CLionProjects/Pacman/SDL2_image-2.8.8/x86_64-w64-mingw32")
set(SDL2_IMAGE_INCLUDE_DIR "${SDL2_IMAGE_DIR}/include")
set(SDL2_IMAGE_LIB_DIR "${SDL2_IMAGE_DIR}/lib")

include_directories(
        ${SDL2_INCLUDE_DIR}
        ${SDL2_INCLUDE_DIR}/SDL2
        ${SDL2_IMAGE_INCLUDE_DIR}
)

add_executable(Pacman main.cpp
        src/WindowManager.cpp
        include/WindowManager.h
        include/TextureManager.h
        src/TextureManager.cpp
        include/Game.h
        src/Game.cpp
        include/Map.h
        src/Map.cpp
        include/Pacman.h
        src/Pacman.cpp
        include/Ghost.h
        include/Utils.h
        include/Blinky.h
        include/Pinky.h
        include/Inky.h
        include/Clyde.h
        src/Ghost.cpp
        include/Direction.h
        include/Pathfinder.h
        src/Pathfinder.cpp
        src/Blinky.cpp
        src/Pinky.cpp
        include/GhostState.h
        src/Inky.cpp
        src/Clyde.cpp
        include/GhostManager.h
        src/GhostManager.cpp
)

target_link_libraries(Pacman PRIVATE
        mingw32
        ${SDL2_LIB_DIR}/libSDL2main.a
        ${SDL2_LIB_DIR}/libSDL2.a
        ${SDL2_IMAGE_LIB_DIR}/libSDL2_image.a
        setupapi
        imm32
        version
        winmm
)

target_link_options(Pacman PRIVATE -mwindows)
