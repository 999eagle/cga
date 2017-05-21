uniform sampler2DShadow shadowMapTexture;
uniform mat4 lightViewProj;
uniform bool enableShadowMap;

float GetShadowMapValue(Fragment fragment, vec3 lightDir)
{
	float shadow = 0.0;
	if (enableShadowMap)
	{
		vec2 texelSize = 1.0 / textureSize(shadowMapTexture, 0);
		vec4 lightFragment = lightViewProj * vec4(fragment.worldPos, 1.0);
		lightFragment /= lightFragment.w;
		lightFragment.xyz = lightFragment.xyz * 0.5 + 0.5;
		float bias = max(0.05 * (1.0 - dot(fragment.normal, lightDir)), 0.005);
		for (float x = -1.5; x <= 1.5; x++)
		{
			for (float y = -1.5; y <= 1.5; y++)
			{
				shadow += texture(shadowMapTexture, vec3(lightFragment.xy + vec2(x, y) * texelSize, lightFragment.z - bias)); 
			}
		}
		shadow /= 16.0;
	}
	return shadow;
}
