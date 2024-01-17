include(FetchContent)

FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML
    GIT_TAG 2.6.1
)
FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG v1.13.0
)
FetchContent_MakeAvailable(SFML spdlog)

add_executable(${BINARY}
    main.cpp
    app/CircuitEditor.cpp
    asset/deserialize.cpp
    ui/layout/HBox.cpp
    ui/components/ImageView.cpp
    ui/components/CircuitButton.cpp
    ui/components/Button.cpp
    ui/components/Label.cpp
    core/Camera.cpp
    circuit/BinaryGate.cpp
    circuit/NotGate.cpp
    circuit/CompositeCircuit.cpp
    circuit/Pin.cpp
    circuit/Wire.cpp
    tools/PinConnector.cpp
    tools/SelectionTool.cpp
    tools/CircuitDragger.cpp
    util/DragBoard.cpp
    util/util.cpp
)
target_include_directories(${BINARY}
    PUBLIC .
    PUBLIC ${INCLUDE_VENDOR}
)
target_link_libraries(${BINARY} sfml-graphics spdlog)

# Copy resource files to build directory
add_custom_target(assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/data
    ${CMAKE_CURRENT_BINARY_DIR}/data
)
