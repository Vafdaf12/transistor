# Transistor
Transistor is an attempt to make a high-performance digital logic simulator that can be used in an educational context to showcase the complex circuitry behind the concepts taught in computer architecure classes.

![image](https://github.com/Vafdaf12/transistor/assets/25525445/58ce5173-615b-4394-a9ce-5552eb62693b)

# Building
Building this project requires CMake. In most cases you should be able to configure and build the project with
```sh
cmake -S . --preset default 
cmake --build build
```

The build output can be found in `build/src/`. The exact location of the binary depends on your generator

# Usage
By default, Transistor will attempt to open a the file `data/world.json`. This can be overridden by passing a path as an argument when executing Transistor:
```
./Transistor path/to/world.json
```

The basic controls of Transistor are as follows:
- `Up/Down Arrow` increases/decreases the number of output pins respectively
- `Alt +Up/Down Arrow` increases/decreases the number of input pins respectively
- `Control+C`/`Control+V` copies/pastes the selected circuits respectively
- `Delete` deletes the selected circuits

Primitive gates can be dragged and dropped into the world from the top bar. 

## Custom Circuits
Custom circuits are loaded from a a JSON file in the `data` folder. In general, if you have a file `data/xxx.json`, it corresponds to the custom circuit type `xxx`. Currently there is no way to add custom circuits in Transistor through the GUI, requiring some manual text editing:
1. Open a circuit file in the `data` folder, e.g. `data/my_custom_circuit.json`
2. Edit the circuit as you see fit
3. In a different circuit file, manually add the custom circuit in the file by using the format:
```json
{
    "id": "<some-id-here>",
    "type": "my_custom_circuit",
    "position": { "x": 0, "y": 0 }
}
```
4. Once loaded into the world, you can use copy-paste to retrieve multiple circuits

