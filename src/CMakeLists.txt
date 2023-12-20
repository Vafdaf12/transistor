include(FetchContent)

FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML
    GIT_TAG 2.6.1
)
FetchContent_MakeAvailable(SFML)

add_executable(${BINARY} 
    main.cpp
    CircuitButton.cpp
    circuit/NandCircuit.cpp
    circuit/BinaryGate.cpp
    circuit/NotGate.cpp
    pin/Pin.cpp
    pin/Wire.cpp
    game/GameWorld.cpp
    asset/CommandLoader.cpp
    asset/deserialize.cpp
    tools/NavigationTool.cpp
    tools/PinConnector.cpp
    tools/SelectionTool.cpp
    tools/CircuitDragger.cpp
    util/DragBoard.cpp
)
target_include_directories(${BINARY}
    PUBLIC .
    PUBLIC ${INCLUDE_VENDOR}
)
target_link_libraries(${BINARY} sfml-graphics)
