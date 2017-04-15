#version 330 core

in vec2 texCoord;

out vec4 outColor;

uniform sampler2D colorTexture;

const float gamma = 2.2;

void main()
{
	vec4 color = texture(colorTexture, texCoord);
	outColor = vec4(pow(color.rgb, vec3(1.0 / gamma)), color.a);
}
