#version 330 core

layout (location = 0) in vec3 inPosition;

uniform mat4 world;
uniform mat4 viewProj;

void main()
{
	gl_Position = viewProj * world * vec4(inPosition, 1.0);
}
