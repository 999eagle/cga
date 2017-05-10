#version 330 core

in vec2 texCoord;

out vec4 outColor;

uniform sampler2D colorTexture;
uniform sampler2D normalTexture;
uniform sampler2D depthTexture;
uniform mat4 invViewProj;
uniform vec3 cameraPosition;

uniform vec3 lightColor;
uniform vec3 lightDirection;

uniform sampler2D shadowMapTexture;
uniform mat4 lightViewProj;
uniform bool enableShadowMap;

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

//
// BRDF functions
//

const float PI = 3.14159265358979323;
// fresnel term: schlick approximation
vec3 F_Schlick(float ndv, vec3 f0)
{
	return f0 + (1.0 - f0) * pow(1.0 - ndv, 5.0);
}

// distribution function: trowbridge-reitz ggx
float NDF_GGX(float ndh, float alpha)
{
	float a2 = alpha * alpha;
	float ndh2 = ndh * ndh;
	float n = ndh2 * (a2 - 1.0) + 1.0;
	return a2 / (PI * n * n);
}
// geometry function: combination of schlick-beckmann and ggx
float G_SchlickGGX(float ndv, float roughness)
{
	float r = roughness + 1.0;
	float k = r * r / 8.0;
	return ndv / (ndv * (1.0 - k) + k);
}
// geomtry function: smith's method for combining view and light direction geometry
float G_Smith(float ndv, float ndl, float roughness)
{
	return G_SchlickGGX(ndv, roughness) * G_SchlickGGX(ndl, roughness);
}

// cook torrance BRDF
vec3 BRDF_CookTorrance(Fragment fragment, vec3 viewDir, vec3 lightDir)
{
	vec3 h = normalize(viewDir + lightDir);
	float ndl = max(dot(fragment.normal, lightDir), 0.0);
	float ndv = max(dot(fragment.normal, viewDir), 0.0);
	float ndh = max(dot(fragment.normal, h), 0.0);
	float hdv = max(dot(h, viewDir), 0.0);
	float alpha = fragment.roughness * fragment.roughness;

	vec3 f0 = mix(vec3(0.04), fragment.albedo, fragment.metallic);
	
	vec3 F = F_Schlick(hdv, f0);
	vec3 specular = NDF_GGX(ndh, alpha) * F * G_Smith(ndv, ndl, fragment.roughness) / (4 * ndv * ndl + 0.001);
	vec3 diffuse = fragment.albedo / PI;

	return ((vec3(1.0) - F) * (1.0 - fragment.metallic) * diffuse + specular) * ndl;
}

void main()
{
	Fragment fragment = ReadGBuffer();
	
	vec3 viewDir = normalize(cameraPosition - fragment.worldPos);
	vec3 lightDir = normalize(-lightDirection);

	if (enableShadowMap)
	{
		vec4 lightFragment = lightViewProj * vec4(fragment.worldPos, 1.0);
		lightFragment /= lightFragment.w;
		lightFragment.xyz = lightFragment.xyz * 0.5 + 0.5;
		float lightDepth = texture(shadowMapTexture, lightFragment.xy).r;
		float bias = max(0.05 * (1.0 - dot(fragment.normal, lightDir)), 0.005);
		if (lightFragment.z - bias > lightDepth)
		{
			discard;
		}
	}

	vec3 brdf = BRDF_CookTorrance(fragment, viewDir, lightDir);

	brdf *= lightColor;

	outColor = vec4(brdf, 1.0);
}
