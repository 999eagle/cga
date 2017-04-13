#version 330 core

in vec2 texCoord;

out vec4 outColor;

uniform sampler2D colorTexture;
uniform sampler2D normalTexture;
uniform sampler2D depthTexture;

uniform vec3 lightColor;

void main()
{
	vec4 colorData = texture(colorTexture, texCoord);
	vec4 normalData = texture(normalTexture, texCoord);
	float depth = texture(depthTexture, texCoord).r;
	if(depth == 1.0)
	{
		discard;
	}

	outColor = vec4(colorData.rgb * lightColor, 1.0);
}
