#version 440 core

layout(location = 0) in vec3 pos;

out vec2 uv;

void main()
{
	gl_Position = vec4(pos, 1.0);
	uv = 0.5*(pos.xy+1.0);
}
