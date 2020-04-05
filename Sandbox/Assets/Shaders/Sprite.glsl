#type vertex

    #version 410 core

    layout (location = 0) in vec3 a_Position;
    layout (location = 1) in vec4 a_Color;
    layout (location = 2) in vec2 a_TextureCoord;
    layout (location = 3) in float a_TextureIndex;
    
    out VertexOutput
    {
        vec4  Color;
        vec2  TextureCoord;
        float TextureIndex;
    }
    vertexOuput;

    uniform mat4 u_ViewProj;
    // uniform vec3 u_CameraPosition;

    void main()
    {
        vertexOuput.Color = a_Color;
        vertexOuput.TextureCoord = a_TextureCoord;
        vertexOuput.TextureIndex = a_TextureIndex;

        gl_Position = u_ViewProj * vec4(a_Position, 1.0f);
    }

#type fragment

    #version 410 core

    layout (location = 0) out vec4 color;

    in VertexOutput
    {
        vec4  Color;
        vec2  TextureCoord;
        float TextureIndex;
    }
    fragmentInput;
    
    uniform sampler2D u_Textures[gl_MaxTextureImageUnits];

    void main()
    {
        int textureIndex = int(fragmentInput.TextureIndex);
        color = texture(u_Textures[textureIndex], fragmentInput.TextureCoord) * fragmentInput.Color;
    }