#version 460 core
layout (location = 0) in vec3 aPos;
uniform vec3 m_gizmoColor;
out vec4 FragColor;

uniform mat4 g_Model;
uniform mat4 g_View;
uniform mat4 g_Projection;

void main()
{
    gl_Position = g_Projection * g_View * g_Model * vec4(aPos, 1.0);
    FragColor = vec4(m_gizmoColor, 1.0);
}