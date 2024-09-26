#version 460 core

layout (location = 0) in vec3 aPos;       // Vertex position
layout (location = 1) in vec3 aNormal;    // Vertex normal

out vec3 FragPos;                          // Fragment position
out vec3 Normal;                           // Normal vector
out vec3 objectColor;                      // Pass the object color

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 objectColorUniform;           // Object color

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal; // Transform normal
    objectColor = objectColorUniform;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
