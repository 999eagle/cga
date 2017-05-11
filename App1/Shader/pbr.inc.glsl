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
