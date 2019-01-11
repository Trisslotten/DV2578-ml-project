#version 440 core

layout(location = 0) in vec3 pos;

out vec3 vpos;
out vec3 vdir;
out vec2 uv;

uniform mat4 viewProj;
uniform vec3 cameraPos;



void main()
{
	vdir = normalize(inverse(mat3(viewProj)) * (pos+vec3(0,0,1.0)));
	vpos = vdir + cameraPos;
	gl_Position = vec4(pos, 1.0);
	uv = 0.5*(pos.xy+1.0);
}
