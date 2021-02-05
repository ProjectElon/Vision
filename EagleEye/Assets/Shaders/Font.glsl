#type vertex

#version 410 core

layout (location = 0) in vec2  a_Position;
layout (location = 1) in vec4  a_Color;
layout (location = 2) in vec2  a_TextureCoord;
layout (location = 3) in float a_TextureIndex;
layout (location = 4) in float a_EntityIndex;

out VertexOutput
{
    vec4  Color;
    vec2  TextureCoord;
    float TextureIndex;
    float EntityIndex;
}
vertexOuput;

uniform mat4 u_ViewProj;

void main()
{
    vertexOuput.Color        = a_Color;
    vertexOuput.TextureCoord = a_TextureCoord;
    vertexOuput.TextureIndex = a_TextureIndex;
    vertexOuput.EntityIndex  = a_EntityIndex;

    gl_Position = u_ViewProj * vec4(a_Position, 0.0f, 1.0f);
}

#type fragment

#version 410 core

layout (location = 0) out vec4 color;
layout (location = 1) out int entityIndex;

in VertexOutput
{
    vec4  Color;
    vec2  TextureCoord;
    float  TextureIndex;
    float  EntityIndex;
}
fragmentInput;

uniform sampler2D u_Textures[gl_MaxTextureImageUnits];

void main()
{
    for (uint i = 0; i < gl_MaxTextureImageUnits; ++i)
    {
        if (fragmentInput.TextureIndex == i)
        {
            vec4 font_texel = texture(u_Textures[i], fragmentInput.TextureCoord);
            color = vec4(1.0f, 1.0f, 1.0f, font_texel.a) * fragmentInput.Color;
            break;
        }
    }

    entityIndex = int(fragmentInput.EntityIndex);
}