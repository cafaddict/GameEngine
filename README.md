# Vulkan based game engine

Game Engine development using vulkan API

## Features (Implemented)

- Basic rendering with Vulkan API
- basic logging
- Entity Component System to manage entities
- Asset Manager to managed assets (model, texuture, shaders)
- Model import

## Features (To-do)

- more UI for Editor feature
- Physcis Engine (maybe import Bullet?)

## Getting Started

I tried to include most of libraries in the project except Vulkan, C++ Compiler, and CMake, but some libraries may be need to installed by you.

### Prerequisites

- **CMake v3.15+** - found at [https://cmake.org/](https://cmake.org/)

- **C++ Compiler** - needs to support at least the **C++20** standard, i.e. _MSVC_,
  _GCC_, _Clang_

- **VulaknSDK** - found at [LunarG](https://www.lunarg.com/vulkan-sdk/)

## How to build

clone the project from
[GitHub](https://github.com/cafaddict/GameEngine)

or just do

```bash
git clone https://github.com/cafaddict/GameEngine
```

after cloning the project, to build the project, all you need to do is run a smiliar **CMake** routine to the one below;

## Building the project

```bash
mkdir build/ && cd build/
cmake .. -
make
```

## Running the examples

currently, I am working on the examples in SandBox. You can import your model and test.

```bash
cd build/SandBox
./SandBox

```

## Authors

- **Hyunyul Cho** - [@cafaddict](https://github.com/filipdutescu)
