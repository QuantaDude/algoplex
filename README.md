# Algorithm visualizer

---

## What is it?

Algo Visualizer is a web app which aims to teach data structures and algorithms in an intuitive and easy to understand manner. It is built using C++, WASM, Raylib, React and TypeScript.

This project combines the high performance C++, WASM code for rendering, and TypeScript and React for the rapid and ease of development of User Interface. Combining the best of both worlds. 

## Algorithms

- [x] DFS
- [ ] BFS
- [ ] TSP algorithms
- [ ] More in the future.

## Features

- [x] Visualize Tree & Graph scenes.
- [x] Add and manipulate nodes on the 2D canvas.
- [x] Add, edit, and remove edges between nodes.
- [x] Step through the algorithm.
- [ ] Step through pseudo code and highlight.
- [ ] Write algorithms at runtime and debug the program.

## Building

for WASM, set the env variables or source from the shell script,then:

```
# emcmake cmake -B build_wasm -DCMAKE_BUILD_TYPE=Release -DOPENGL_VERSION="ES 3.0"
# cmake --build build_wasm -j$(nproc)
# emrun build_wasm/mygame.html
```

# IMPORTANT!

To ship the app:
In Cmakelists.txt, set the PRODUCTION_BUILD flag to ON to build a shippable version of your game. This will change the file paths to be relative to your exe (RESOURCES_PATH macro), will remove the console, and also will change the asserts to not allow people to debug them. To make sure the changes take effect I recommend deleting the out folder to make a new clean build!

Also, if you read the CMAKE, even if you don't know CMAKE you should understand what happens with the comments there and you can add libraries and also remove the console from there if you need to! (there is a commented line for that!)

