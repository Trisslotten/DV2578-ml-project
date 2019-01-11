#version 440 core

out vec4 outColor;

in vec3 vpos;
in vec3 vdir;
in vec2 uv;


uniform sampler2D tex;
uniform float time;
uniform float simTime;
uniform vec3 cameraPos;
uniform float numPasses;
uniform float numFrames;
uniform int samplesPerPass;

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

mat3 castRay(Ray ray, out bool isLight)
{
	isLight = false;
	float t = -1.0;
	vec3 n = vec3(0);
	vec3 color = vec3(0);
	for(int i = 0; i < 5; i++)
	{
		vec3 pos = vec3(0);
		float usedTime = simTime;
		pos.x = 3*sin(usedTime *0.7 + 10*i);
		pos.y = 3*sin(usedTime *0.5 - 5*i);
		pos.z = 3*sin(usedTime *0.35 + 5*i);
		float radius = 1;
		float nt = 0.;
		vec3 nn = vec3(0);
		if(sphere(ray, pos, radius, nt, nn) && (nt < t || t < 0))
		{
			t = nt;
			n = nn;
			color = vec3(1);
			if(i == 0)
			{
				color = vec3(0.7);
				isLight = true;
			} else {
				isLight = false;
			}
		}
	}

	float nt = 0.;
	if(plane(ray, vec3(0, -4.5, 0), vec3(0,1,0), nt) && (nt < t || t < 0))
	{
		t = nt;
		n = vec3(0,1,0);
		color = vec3(1);
	}
	if(plane(ray, vec3(0, 4.5, 0), vec3(0,-1,0), nt) && (nt < t || t < 0))
	{
		t = nt;
		n = vec3(0,-1,0);
		color = vec3(1);
	}
	if(plane(ray, vec3(-4.5, 0, 0), vec3(1,0,0), nt) && (nt < t || t < 0))
	{
		t = nt;
		n =vec3(1,0,0);
		color = vec3(1.0, 0.5, 0.5);
	}
	if(plane(ray, vec3(4.5, 0, 0), vec3(-1,0,0), nt) && (nt < t || t < 0))
	{
		t = nt;
		n = vec3(-1,0,0);
		color = vec3(0.5, 1.0, 0.5);
	}
	if(plane(ray, vec3(0, 0, 4.5), vec3(0,0,-1), nt) && (nt < t || t < 0))
	{
		t = nt;
		n = vec3(0,0,-1);
		color = vec3(1);
	}

	mat3 result = mat3(0);
	if(t > 0)
	{
		result[0] = ray.o + t*ray.d;
		result[1] = n;
		result[2] = color;
	}
	return result;
}


/*
*********************************************************
*********************************************************
*/
const float PI = 3.141592;
// http://www.amietia.com/lambertnotangent.html
vec3 cosUnitHemisphere(vec2 seed, vec3 n) {
    float t = hash12(seed)*(2.0*PI);
    float u = 2*hash12(seed + 2000.0)-1;
    return normalize(n + vec3(vec2(cos(t), sin(t)) * sqrt(1.0 - u*u), u));
}
/*
*********************************************************
*********************************************************
*/


const int MAX_BOUNCES = 4;

vec3 findColor(Ray ray, vec2 seed)
{
	vec3 color = vec3(0);

	vec3 colorMask = vec3(1);
	for(int i = 0; i < MAX_BOUNCES+1; i++)
	{
		bool isLight = false;
		mat3 data = castRay(ray, isLight);
		if(data[1] == vec3(0))
			break; // no hit
		// move the origin a little to avoid intersection at same point
		ray.o = data[0] + data[1]*0.001;
		colorMask *= data[2];
		if(isLight)
		{
			color += colorMask * data[2];
			break;
		}

		ray.d = cosUnitHemisphere(seed + vec2(2000*i,0), data[1]);
	}
	return color;
}

void main()
{
	vec3 dir = normalize(vdir);

	vec3 color = vec3(0);
	Ray ray;
	ray.o = cameraPos;
	ray.d = dir;

	for(int i = 0; i < samplesPerPass; i++)
	{
		color += findColor(ray, 4000*uv + vec2(50,1000*i+50) + time);
	}
	color /= float(samplesPerPass) * (1.0 / (2.0 * PI));


	vec3 prevColor = texture(tex, uv).rgb;
	
	if(numFrames < numPasses)
	{
		color /= numPasses;
		if(numPasses != 1.)
		{
			color += prevColor;
		}
	}
	else 
	{
		color = prevColor;
	}

	outColor = vec4(color, 1.0);
}