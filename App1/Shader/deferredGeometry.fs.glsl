#version 330 core

in mat3 NTB;
in vec2 texCoord;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 outNormal;

uniform sampler2D diffuseTexture0;
uniform sampler2D normalTexture0;
uniform sampler2D specularTexture0;

void main()
{
	outColor.rgb = texture(diffuseTexture0, texCoord).rgb;
	vec3 normal = texture(normalTexture0, texCoord).rgb;
	vec3 specular = texture(specularTexture0, texCoord).rgb;
	outColor.a = specular.r;
	outNormal.xyz = normalize(NTB * normalize(normal * 2.0 - 1.0));
}
