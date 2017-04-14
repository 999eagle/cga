#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in vec3 inBitangent;

uniform mat4 viewProj;
uniform mat4 world;

out vec2 texCoord;
out mat3 NTB;

void main()
{
	gl_Position = viewProj * world * vec4(inPosition, 1.0);
	texCoord = inTexCoord;
	vec3 N = normalize(vec3(world * vec4(inNormal, 0.0)));
	vec3 T = normalize(vec3(world * vec4(inTangent, 0.0)));
	vec3 B = normalize(vec3(world * vec4(inBitangent, 0.0)));
	NTB = mat3(T, B, N);
}
