#version 330 core

in vec2 texCoord;

out vec4 outColor;

uniform sampler2D colorTexture;

void main()
{
	outColor = texture(colorTexture, texCoord);
}
