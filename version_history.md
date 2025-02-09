# Update 0.0.2 - 04.05.2024

1. Fixed the initialization of the cube and the shader
2. Created camera script to accommodate the camera view
3. Add Check for errors for the following:
- Framebuffer, Cube, Imgui, Shader, Depth Testing
4. Updated Scene View
5. Fixed minimizing causing crash

# Update 0.0.3 - 04.07.2024
**1. A lot of changes have been made:**
 - Fixed the memory leak caused by the framebuffer
 - Redefined GameObject class
 - Fixed grid to be on XZ plane instead of what it was because it caused issues
 - Changed to "Component-Based Design(CBD)"
 - Moved data to src subdir for better visibility
 - Still some of the new files are empty like Renderer and Utils

**2. Not all the changes are written here.**

# TODO:
    Still GUI and Triangle classes are unused! Make main.cpp less code, implement rendering in its class file
    
# Update 0.0.6 - 01.25.2025
**1. A lot lot of changes have been made so far:**
 - Many issues has been fixed
 - Swithched from forward to deferred rendering
 - Significantly optimized the Mesh system 
 - All of the standard objects like cube, capsule and so on had over 65000 vertices and 200k indices for each type object --fixed
 - Rewritten the project explorer and assetmanager structure
 - Added Makefile 
 - Added Compatibility with Linux OS
 - Created global lighting system alongside the new renderer
 - Moved the framebuffer implementation to Scene class
 - Fixed the fps drop that were on the previous patch 0.0.5.. caused the fps to drop below 5
 - Rewrriten thread system
 - Optimized shaders and their recursive func for finding them in real time
 - Added more logs
 - Fixed CMakeLists configuration --added sanitizers support for both OS
 - moved some definitions from Initialization to main to maintain with the thread system: TODO- move them again to Init back
 - Fixed significant memory leak from the thread system
 - Rewritten basic and lighting shaders for the new rendering system
 - Removed the Profilier for now 
 - For each of the objects, removed their implementation in their main classes. Now creating objects is happening in the Scene class
 - Fixed the transparency caused by the new rendering system for part of the objects
 - Fixed camera yaw and pitch to update correctly the forward vector
 - TODO: still there is one more memory leak caused by new_allocator.h from object creation that need to be fixed urgently
 - Optimized the framebuffer performance
 - Switch gameobject's to inherit Components, not GameObject class
 - Integrated wireframe directly into geometry shader

# Update 0.0.61 - 02.09.2025
**1. Minor changes:**
- Added Shader Manager class
- Optimized the structure
- Added my own library for managing the engine (closed source)
- Changed the imgui layout and fixed docking
- Fixed DrawGrid and Capsule Collider - removed the deprecated immediate mode rendering
- Forced of use of OpenGL 4.6 and glad core profile
- Made significant optimization of the CPU and RAM usage (more in the future)
- Added Profiler
- Some changes in the Project explorer
- and few more