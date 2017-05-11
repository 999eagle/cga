#version 330 core

in vec2 texCoord;

out vec4 outColor;

uniform sampler2D colorTexture;

uniform float exposure;

void main()
{
	vec4 color = texture(colorTexture, texCoord);
	outColor = vec4(vec3(1.0) - exp(-color.rgb * exposure), color.a);
}
