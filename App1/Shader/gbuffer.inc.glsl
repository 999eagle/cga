uniform sampler2D colorTexture;
uniform sampler2D normalTexture;
uniform sampler2D depthTexture;
uniform mat4 invViewProj;

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
