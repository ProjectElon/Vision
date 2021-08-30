#type vertex

#version 410 core

layout (location =  0) in vec3 a_Position;
layout (location =  1) in vec2 a_TextureCoord;
layout (location =  2) in vec3 a_Normal;

out VertexOutput
{
    vec2      TextureCoord;
}
vertexOutput;

uniform mat4 u_Transform;
uniform mat4 u_ViewProj;

void main()
{
    vertexOutput.TextureCoord = a_TextureCoord;
    gl_Position = u_ViewProj * u_Transform * vec4(a_Position, 1.0f);
}

#type fragment

#version 410 core

layout (location = 0) out vec4 color0;
layout (location = 1) out int color1;

in VertexOutput
{
    vec2 TextureCoord;
}
fragmentInput;

uniform sampler2D u_Texture;
uniform int u_EntityID;

void main()
{
    color0 = texture(u_Texture, fragmentInput.TextureCoord);
    color1 = u_EntityID;
}