#version 330 core

in vec2 texCoord;

out vec4 outColor;

uniform sampler2D colorTexture;
uniform sampler2D normalTexture;
uniform sampler2D depthTexture;
uniform mat4 invViewProj;
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

Fragment ReadGBuffer()
{
	vec4 colorData = texture(colorTexture, texCoord);
	vec4 normalData = texture(normalTexture, texCoord);
	float depth = texture(depthTexture, texCoord).r;
	// discard far fragments --> skybox can be pre-rendered
	if(depth == 1.0)
	{
		discard;
	}
	// reconstruct world-space position of the current fragment
	vec3 clipPos = vec3(texCoord, depth) * 2.0 - 1.0;
	vec4 worldPos = (invViewProj * vec4(clipPos, 1.0));

	Fragment fragment;
	fragment.worldPos = worldPos.xyz / worldPos.w;
	fragment.albedo = colorData.rgb;
	fragment.normal = normalData.xyz;
	fragment.roughness = colorData.a;
	fragment.metallic = normalData.w;
	return fragment;
}

void main()
{
	Fragment fragment = ReadGBuffer();

	outColor = vec4(fragment.albedo * lightColor, 1.0);
}
