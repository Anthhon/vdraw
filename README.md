# vdraw

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Raylib](https://img.shields.io/badge/Raylib-5.0-blue.svg)](https://www.raylib.com/)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey)](https://github.com/yourusername/vdraw)

**vdraw** is an interactive 3D graphing tool built with Raylib that allows you to visualize mathematical functions in real-time. Plot parametric curves in 3D space with an intuitive camera system and dynamic controls.

![vdraw Screenshot](screenshot.png)

## Features

- **Real-time 3D visualization** of parametric functions
- **Interactive orbital camera** with smooth inertia and damping
- **Dynamic resolution control** - adjust the number of plotted points on-the-fly
- **Custom parameter control** - modify function parameters in real-time
- **Cinematic mode** - automatic camera rotation for dynamic viewing
- **Coordinate axes** and grid overlay for spatial reference
- **Performance metrics** - real-time FPS counter with color coding
- **Custom font support** - clean typography for UI elements
- **Intuitive controls** - keyboard and mouse driven interaction

## Quick Start

### Prerequisites

- **Raylib 5.0** or later
- **C compiler** (GCC, Clang, or MSVC)
- **CMake** (optional, for building)
- **GLFW3** (included with raylib)

### Installation

#### Linux (Ubuntu/Debian)

```bash
# Install Raylib dependencies
sudo apt-get install libglfw3-dev libopenal-dev libvulkan-dev

# Clone the repository
git clone https://github.com/yourusername/vdraw.git
cd vdraw

# Compile with GCC
gcc -o vdraw vdraw.c -lraylib -lm -lglfw -lpthread -ldl -lX11

# Run the visualizer
./vdraw
```

#### Windows (MinGW)

```bash
# Assuming Raylib is installed in C:\raylib
gcc -o vdraw.exe vdraw.c -IC:\raylib\include -LC:\raylib\lib -lraylib -lopengl32 -lgdi32 -lwinmm
vdraw.exe
```

#### macOS

```bash
# Install Raylib via Homebrew
brew install raylib

# Compile
gcc -o vdraw vdraw.c -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

# Run
./vdraw
```

## Customizing Functions

The visualizer is designed to make function experimentation easy. Edit the macro definitions at the top of `vdraw.c` to visualize any parametric function:

```c
#define X_FUNC_INPUT cos(t)      // X coordinate function
#define Y_FUNC_INPUT sin(t)      // Y coordinate function  
#define Z_FUNC_INPUT t*m         // Z coordinate function (with parameter 'm')
```
