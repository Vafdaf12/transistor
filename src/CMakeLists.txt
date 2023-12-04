include(FetchContent)

FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML
    GIT_TAG 2.6.1
)
FetchContent_MakeAvailable(SFML)

add_executable(${BINARY} 
    main.cpp
    ComponentDragger.cpp
    Circuit.cpp
    pin/Pin.cpp
    pin/Wire.cpp
)
target_include_directories(${BINARY}
    PRIVATE src/
)
target_link_libraries(${BINARY} sfml-graphics)