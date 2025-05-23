#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture samplers
uniform sampler2D texture_num;

void main()
{
	vec4 texColor = texture(texture_num, TexCoord);
	if(texColor.a < 0.1)
		discard;
	FragColor = texColor;
}