#version 330 core

in vec2 texCoord;

out vec4 outColor;

uniform vec3 cameraPosition;
uniform vec3 lightColor;
uniform vec3 lightDirection;

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
#include "shadowMap"

void main()
{
	Fragment fragment = ReadGBuffer();
	
	vec3 viewDir = normalize(cameraPosition - fragment.worldPos);
	vec3 lightDir = normalize(-lightDirection);

	vec3 light = BRDF_CookTorrance(fragment, viewDir, lightDir) * GetShadowMapValue(fragment, lightDir);

	light *= lightColor;

	outColor = vec4(light, 1.0);
}
