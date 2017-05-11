#version 330 core

in vec2 texCoord;

out vec4 outColor;

uniform sampler2D colorTexture;
uniform sampler2D bloomTexture;

void main()
{
	outColor = texture(colorTexture, texCoord) + texture(bloomTexture, texCoord);
}
