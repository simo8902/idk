#version 460 core
layout (location = 0) in vec3 aPos;
uniform vec3 m_wireframeColor;
out vec4 FragColor;

uniform mat4 m_Model;
uniform mat4 m_View;
uniform mat4 m_Projection;

void main()
{
    gl_Position = m_Projection * m_View * m_Model * vec4(aPos, 1.0);
    FragColor = vec4(m_wireframeColor, 1.0);
}