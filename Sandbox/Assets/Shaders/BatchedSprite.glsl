#type vertex

    #version 410 core

    layout (location = 0) in vec3 Position;
    layout (location = 1) in vec4 Color;
    layout (location = 2) in vec2 TexCoord;
    layout (location = 3) in float TexID;

    out vec4  _Color;
    out vec2  _TexCoord;
    out float _TexID;
    
    uniform mat4 u_ViewProj;

    void main()
    {
        _Color = Color;
        _TexCoord = TexCoord;
        _TexID = TexID;

        gl_Position = u_ViewProj * vec4(Position, 1.0f);
    }

#type pixel

    #version 410 core

    in vec4  _Color;
    in vec2  _TexCoord;
    in float _TexID;

    out vec4 Color;

    uniform sampler2D u_Textures[32];

    void main()
    {
        Color = texture(u_Textures[int(_TexID)], _TexCoord) * _Color;
    }