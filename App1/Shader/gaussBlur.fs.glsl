#version 330 core

uniform sampler2D colorTexture;
uniform bool horizontal;

in vec2 texCoord;
out vec4 outColor;

// 9 tap gaussian blur with linear sampling
// source: http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/

uniform float offsets[3] = float[]( 0.0, 1.3846153846, 3.2307692308 );
uniform float weights[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );

void main()
{
	vec2 texelSize = 1.0 / textureSize(colorTexture, 0);
	outColor = texture(colorTexture, texCoord) * weights[0];
	for (int i = 1; i < 5; i++)
	{
		vec2 offset = vec2(0.0);
		if (horizontal)
		{
			offset.x = offsets[i];
		}
		else
		{
			offset.y = offsets[i];
		}
		offset *= texelSize;
		outColor += texture(colorTexture, texCoord + offset) * weights[i];
		outColor += texture(colorTexture, texCoord - offset) * weights[i];
	}
}
