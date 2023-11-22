include(FetchContent)

set(SDL_STATIC ON)
FetchContent_Declare(SDL2 
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG release-2.26.2
)
FetchContent_MakeAvailable(SDL2)

add_executable(${BINARY} 
    main.cpp
)
target_include_directories(${BINARY}
    PRIVATE src/
)
target_link_libraries(${BINARY} SDL2-static)