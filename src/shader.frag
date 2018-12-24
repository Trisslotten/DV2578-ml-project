#version 330 core

out vec4 outColor;

in vec3 vpos;
in vec3 vdir;
in vec2 uv;

uniform float time;

float noise(vec3 p);

float n(vec3 p, float seed)
{
	float freq = 5.0;
	float a = 0.5;
	vec3 v = freq * p;
	float result = 0.0;
	for(int i = 0; i < 5; i++)
	{
		result += a*noise(v + seed);
		v*=2.0;
		a*=0.5;
	}
	return result;
}

void main()
{
	vec3 dir = normalize(vdir);



	vec3 color;
	color.r = n(dir, 0);
	color.g = n(dir, 0);
	color.b = n(dir, 0);

	outColor = vec4(color, 1.0);
}





float mod289(float x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 mod289(vec4 x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 perm(vec4 x){return mod289(((x * 34.0) + 1.0) * x);}

float noise(vec3 p){
    vec3 a = floor(p);
    vec3 d = p - a;
    d = d * d * (3.0 - 2.0 * d);

    vec4 b = a.xxyy + vec4(0.0, 1.0, 0.0, 1.0);
    vec4 k1 = perm(b.xyxy);
    vec4 k2 = perm(k1.xyxy + b.zzww);

    vec4 c = k2 + a.zzzz;
    vec4 k3 = perm(c);
    vec4 k4 = perm(c + 1.0);

    vec4 o1 = fract(k3 * (1.0 / 41.0));
    vec4 o2 = fract(k4 * (1.0 / 41.0));

    vec4 o3 = o2 * d.z + o1 * (1.0 - d.z);
    vec2 o4 = o3.yw * d.x + o3.xz * (1.0 - d.x);

    return o4.y * d.y + o4.x * (1.0 - d.y);
}
