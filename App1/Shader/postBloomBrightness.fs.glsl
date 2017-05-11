#version 330 core

in vec2 texCoord;

out vec4 outColor;

uniform sampler2D colorTexture;

void main()
{
	vec4 color = texture(colorTexture, texCoord);
	float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0)
	{
		outColor = color;
	}
	else
	{
		outColor = vec4(0.0, 0.0, 0.0, color.a);
	}
}
