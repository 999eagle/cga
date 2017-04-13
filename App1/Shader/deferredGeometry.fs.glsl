#version 330 core

in vec3 normal;
in vec2 texCoord;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 outNormal;

uniform sampler2D diffuseTexture;

void main()
{
	outColor.rgb = texture(diffuseTexture, texCoord).rgb;
	outNormal.xyz = normalize(normal);
}
