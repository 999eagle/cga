#version 330 core

in vec2 texCoord;

out vec4 outColor;

uniform vec3 cameraPosition;
uniform vec3 lightColor;

struct Fragment
{
	vec3 worldPos;
	vec3 albedo;
	vec3 normal;
	float roughness;
	float metallic;
};

#include "gbuffer"

void main()
{
	Fragment fragment = ReadGBuffer();

	outColor = vec4(fragment.albedo * lightColor, 1.0);
}
