#type vertex

#version 410 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Color;

out VertexOutput
{
    vec4  Color;
}
vertexOuput;

uniform mat4 u_ViewProj;

void main()
{
    vertexOuput.Color = a_Color;
    gl_Position = u_ViewProj * vec4(a_Position, 1.0f);
}

#type fragment

#version 410 core   

layout (location = 0) out vec4 color;

in VertexOutput
{
    vec4 Color;
}
fragmentInput;

void main()
{
    color = fragmentInput.Color;
}