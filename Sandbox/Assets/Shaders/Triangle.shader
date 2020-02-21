#vertex

    #version 410 core

    layout (location = 0) in vec3 in_position;
    layout (location = 1) in vec2 in_texCoord;
    
    out vec2 texCoord;

    uniform mat4 u_viewProjection;
    uniform mat4 u_transform;

    void main()
    {
        gl_Position = u_viewProjection * u_transform * vec4(in_position, 1.0f);
        texCoord = in_texCoord;
    }
    
#pixel

    #version 410 core

    in vec2 texCoord;

    out vec4 out_color;
    
    uniform sampler2D u_texture0;
    uniform sampler2D u_texture1;

    void main()
    {
        out_color = texture(u_texture1, texCoord) * texture(u_texture0, texCoord);
    }