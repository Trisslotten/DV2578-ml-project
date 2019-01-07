#version 330 core

out vec4 outColor;

in vec3 vpos;
in vec3 vdir;
in vec2 uv;

uniform float time;
uniform vec3 cameraPos;

struct Ray
{
	vec3 o, d;
};


bool sphere(Ray r, vec3 p, float radius, out float t, out vec3 n)
{
	t = dot(r.d, p-r.o);
	if (t < 0)
		return false;
	float depth = length(t*r.d + r.o - p);
	if(depth > radius)
		return false;
	t -= sqrt(radius*radius - depth*depth);
	n = normalize(t*r.d + r.o - p);
	return true;
}

bool plane(Ray r, vec3 p, vec3 n, out float t)
{
	if(abs(dot(r.d, n)) < 0.0001)
		return false;
	t = dot(p-r.o, n)/dot(r.d, n);
	return t >= 0;
}


const vec4 ss[]=vec4[](
	vec4( 5, 5,5, 1),
	vec4( 5, 0,5, 2),
	vec4(-1,2,-5, 1),
	vec4( 1,2,3, 2),
	vec4( 4,-5,-6, 3),
	vec4(-7,-8,9, 4)
);


// iq hash
float uhash12(uvec2 x)
{
	uvec2 q = 1103515245U * ((x >> 1U) ^ (uvec2(x.y, x.x)));
	uint  n = 1103515245U * ((q.x) ^ (q.y >> 3U));
	return float(n) * (1.0 / float(0xffffffffU));
}
float hash12(vec2 x) { return uhash12(uvec2(50.*x)); }
vec3 dither()
{
	vec3 res = vec3(0);
	res.r += mix(-0.5/255.0, 0.5/255.0, hash12(gl_FragCoord.xy));
	res.g += mix(-0.5/255.0, 0.5/255.0, hash12(gl_FragCoord.xy + 1000));
	res.b += mix(-0.5/255.0, 0.5/255.0, hash12(gl_FragCoord.xy + 2000));
	return res;
}

float noise(vec3 p);
vec3 noiseColor(vec3 p)
{
	vec3 col = vec3(0);
	col += 0.5*noise(p);
	col += 0.25*noise(2*p);
	col += 0.125*noise(4*p);
	return col;
}

void main()
{
	vec3 dir = normalize(vdir);

	vec3 color = vec3(1);
	Ray ray;
	ray.o = cameraPos;
	ray.d = dir;
	float t = -1.0;
	vec3 n = vec3(0);
	for(int i = 0; i < ss.length(); i++)
	{
		float nt = 0.;
		vec3 nn = vec3(0);
		if(sphere(ray, ss[i].xyz, ss[i].w, nt, nn) && (nt < t || t < 0))
		{
			t = nt;
			n = nn;
		}
	}

	float nt = 0.;
	if(plane(ray, vec3(0, -10, 0), vec3(0,1,0), nt) && (nt < t || t < 0))
	{
		t = nt;
		n = vec3(0,1,0);
	}

	vec3 p = t*ray.d + ray.o;

	if(t > 0.0)
	{
		const vec3 l = normalize(vec3(1,1,0));

		color = noiseColor(p) * (max(dot(l, n), 0.)*0.9 + 0.1);

	}

	color += dither();
	outColor = vec4(color, 1.0);

}

// https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
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
