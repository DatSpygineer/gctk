# GCTk (Game Creation Toolkit)

GCTk is a library for making games written in C with Lua bindings. This library uses OpenGL 4.6 for rendering.


The project is going under the name "cgctk", because this is the C version.

This project is in a very early stage, it is not feature complete and breaking updates may occur.

## Features:

* ❌ Settings
* ✔️ Basic 2D/3D mesh rendering
* ✔️ Rendering of 2D sprites
* ❌ Tilemap rendering
* ❌ Sprite animations
* ❌ Sprite batching
* ✔️ Input system
* ❌ ECS and scene format
* ❌ Fully featured lua bindings
* ❌ Editor application
* ❌ Documentations

## Dependencies:
- OpenGL
- GLFW
- GLEW
- LuaJIT

### Submodules:
- cglm

## How to build:
For building you need CMake 3.25 or newer.
Clone the repository with the recursive flag, so the submodules gets cloned as well.
```sh
git clone https://github.com/DatSpygineer/gctk --recursive
```

### Windows:
You need to install VCPKG for dependencies. The project already contains a toolchain file to find VCPKG.
Make sure you have the required dependencies installed.

Create a build folder and enter
```sh
mkdir buid
cd build
```

Then run cmake and make to start building
```sh
cmake ..
cmake --build .
```

If you don't want to add Lua bindings, you can disable it with a CMake flag
```
cmake .. -DGCTK_ENABLE_LUA=OFF
```

### Linux:
Make sure you have the dependencies listed above installed.
Create a build folder and enter
```sh
mkdir buid
cd build
```

Then run cmake and make to start building
```sh
cmake ..
make all
```

If you don't want to add Lua bindings, you can disable it with a CMake flag
```
cmake .. -DGCTK_ENABLE_LUA=OFF
```
