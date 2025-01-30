#version 330 core

in vec3 FragPos;
in vec3 Normal;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

uniform vec3 objectColor;
uniform vec3 wireframeColor;
uniform bool wireframe;

void main()
{
    if (wireframe) {
        // Render the wireframe color
        gPosition = FragPos;
        gNormal = normalize(Normal);
        gAlbedoSpec = vec4(wireframeColor, 1.0);  // Wireframe color
    } else {
        // Normal G-buffer rendering
        gPosition = FragPos;
        gNormal = normalize(Normal);
        gAlbedoSpec = vec4(objectColor, 1.0);
    }

}
