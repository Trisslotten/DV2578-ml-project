#version 440 core

layout(location = 0) out vec4 outColor;

uniform sampler2D tex;
uniform float numPasses;
uniform float numFrames;

in vec2 uv;

void main()
{
	vec3 color = texture(tex, uv).rgb;
	//color *= numPasses / numFrames;
	//color = pow(color, vec3(1./0.2));
	outColor = vec4(color, 1.0);
}