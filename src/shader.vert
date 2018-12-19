#version 440 core

layout(location = 0) in vec3 pos;

out vec3 vpos;
out vec2 uv;

uniform mat4 viewProj;

void main()
{
	vec4 p = inverse(viewProj) * vec4(pos, 1.0);
	vpos = p.xyz / p.w;
	gl_Position = vec4(pos, 1.0);
	uv = 0.5*(pos.xy+1.0);
}
