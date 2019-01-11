#version 440 core

layout(location = 0) out vec4 outColor;

uniform sampler2D tex;

in vec2 uv;

void main()
{
	vec3 color = texture(tex, uv).rgb;
	outColor = vec4(color, 1.0);
}