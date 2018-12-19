#version 440 core

out vec4 outColor;

in vec3 vpos;
in vec2 uv;

uniform float time;


void main()
{
	outColor = vec4(uv, 0.0, 1.0);
}