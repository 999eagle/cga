#version 330 core

in vec2 texCoord;

out vec4 outColor;

uniform vec3 cameraPosition;
uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform float lightRadius;

struct Fragment
{
	vec3 worldPos;
	vec3 albedo;
	vec3 normal;
	float roughness;
	float metallic;
};

#include "gbuffer"
#include "pbr"

void main()
{
	Fragment fragment = ReadGBuffer();

	vec3 viewDir = normalize(cameraPosition - fragment.worldPos);
	vec3 lightDir = normalize(lightPosition - fragment.worldPos);

	float distance = length(fragment.worldPos - lightPosition) / lightRadius;
	float attenuation = 1.0 / (distance * distance);
	if (attenuation < 0.01)
	{
		discard;
	}

	vec3 brdf = BRDF_CookTorrance(fragment, viewDir, lightDir);

	brdf *= attenuation * lightColor;

	outColor = vec4(brdf, 1.0);
}
