#version 410 core
out vec4 fColor;

in vec2 passTexture;

uniform sampler2D alphaTexture;

void main()
{
	if(texture(alphaTexture, passTexture).r < 0.1)
		discard;

	fColor = vec4(1.0f);
}