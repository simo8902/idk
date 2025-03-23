# idk game engine (name in progress...)

![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/simo8902/LupusFire-Engine)
![GitHub last commit](https://img.shields.io/github/last-commit/simo8902/LupusFire-Engine)
![Lines of Code](https://img.shields.io/badge/Lines%20of%20Code-4763-green)

Simple 3D Game Engine based on a OpenGL - GLFW3, GLM, IMGUI (1.91.2)

## 🔧 Compiling GLFW  

### Prerequisites  
Make sure you have the following installed:  
- CMake from 3.27 to 3.30.5 max supported
- A C++ compiler (GCC, Clang, LLVM) 
- Git (to clone the repo)

### Steps to Compile  

1. **Clone the repository**  
   ```bash
   git clone --recursive https://github.com/simo8902/glfw3.git
   cd glfw3
   ```

2. **Create a build directory**  
   ```bash
   mkdir build
   cd build
   ```

3. **Generate project files using CMake**  
   - **For Windows (MSVC)** unsupported  
     ```bash
     cmake .. -G "Visual Studio 17 2022"
     ```
   - **For Linux/macOS (Makefile)** TBD
     ```bash
     cmake .. -DCMAKE_BUILD_TYPE=Release
     ```

4. **Build the library**  
   - **For Windows (MSVC)** unsupported
     ```bash
     cmake --build . --config Release
     ```
   - **For Linux/macOS** TBD 
     ```bash
     make -j$(nproc)
     ```

5. **Install (Optional, requires admin/sudo)**  
   ```bash
   cmake --install .
   ```

