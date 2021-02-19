#type vertex

#version 410 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Color;
layout (location = 2) in vec2 a_TextureCoord;
layout (location = 3) in uint a_TextureIndex;
layout (location = 4) in int  a_EntityIndex;

out VertexOutput
{
    vec4      Color;
    vec2      TextureCoord;
    flat uint TextureIndex;
    flat int  EntityIndex;
}
vertexOutput;

uniform mat4 u_ViewProj;

void main()
{
    vertexOutput.Color        = a_Color;
    vertexOutput.TextureCoord = a_TextureCoord;
    vertexOutput.TextureIndex = a_TextureIndex;
    vertexOutput.EntityIndex  = a_EntityIndex;

    gl_Position = u_ViewProj * vec4(a_Position, 1.0f);
}

#type fragment

#version 410 core

layout (location = 0) out vec4 color0;
layout (location = 1) out int  color1;

in VertexOutput
{
    vec4      Color;
    vec2      TextureCoord;
    flat uint TextureIndex;
    flat int  EntityIndex;
}
fragmentInput;

uniform sampler2D u_Textures[gl_MaxTextureImageUnits];

void main()
{
    for (uint i = 0; i < gl_MaxTextureImageUnits; ++i)
    {
        if (fragmentInput.TextureIndex == i)
        {
            color0 = texture(u_Textures[i], fragmentInput.TextureCoord)
            * fragmentInput.Color;
            break;
        }
    }

    color1 = fragmentInput.EntityIndex;
}