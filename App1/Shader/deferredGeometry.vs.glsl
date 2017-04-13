#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;

uniform mat4 viewProj;
uniform mat4 world;

out vec3 normal;
out vec2 texCoord;

void main()
{
	gl_Position = viewProj * world * vec4(inPosition, 1.0);
	texCoord = inTexCoord;
	normal = inNormal;
}
