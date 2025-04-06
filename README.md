# idk game engine (name in progress...)

![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/simo8902/LupusFire-Engine)
![GitHub last commit](https://img.shields.io/github/last-commit/simo8902/LupusFire-Engine)
![Lines of Code](https://img.shields.io/badge/Lines%20of%20Code-7587-green)

Simple 3D Game Engine based on a OpenGL - GLFW3, GLM, IMGUI (1.91.2)

## 🔧 Compiling idk ..  

### Prerequisites  
Make sure you have the following installed:  
- CMake from 3.27 to 3.30.5 max supported
- A C++ compiler (GCC, Clang, LLVM) 
- Git (to clone the repo)

### Steps to Compile  

``` 
mkdir build
cd build
cmake -B build
cmake --build build
```
or using the not ready Makefile
```
make
```

### builded with:
compiler: clang64 version - 19.1.6
for target: x86_64-w64-windows-gnu
model: posix