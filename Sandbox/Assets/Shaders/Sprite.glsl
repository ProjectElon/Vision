#type vertex
	
	#version 410 core

	layout (location = 0) in vec3 Position;
	layout (location = 1) in vec2 TexCoord;

	out vec2 _TexCoord;

	/* uniforms */
	uniform mat4 u_Transform;
	uniform mat4 u_ViewProj;

	void main()
	{
		_TexCoord = TexCoord;
		gl_Position = u_ViewProj * u_Transform * vec4(Position, 1.0f);
	}

#type pixel

	#version 410 core

	in vec2 _TexCoord;
	out vec4 _Color;

	/* uniforms */
	uniform vec4 u_TintColor;
	uniform sampler2D u_MainTex;

	void main()
	{
		_Color = texture(u_MainTex, _TexCoord) * u_TintColor;
	}