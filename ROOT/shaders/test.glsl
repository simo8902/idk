#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;

out vec3 vertexColor;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vertexColor = aColor;
    texCoord = aTexCoord;
}

#shader fragment
#version 330 core
in vec3 vertexColor;
in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D albedoTexture;
uniform vec3 baseColor;

void main()
{
    vec4 texColor = texture(albedoTexture, texCoord);
    FragColor = texColor * vec4(baseColor, 1.0) * vec4(vertexColor, 1.0);
}

