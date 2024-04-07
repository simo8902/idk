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
