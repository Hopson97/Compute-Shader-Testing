#version 460 core

layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform writeonly image2D out_image;

// Kind of SDF to render based on selection from the GUI
#define KIND_TORUS 0
#define KIND_CUBES 1
#define KIND_FRACTAL1 2
#define KIND_FRACTAL2 3
#define KIND_FRACTAL_MIX 4
#define KIND_FRACTAL3 5
#define KIND_FRACTAL4 6
#define KIND_FRACTAL5 7
#define KIND_FRACTAL_MIX_1_AND_4 8
#define KIND_FRACTAL6 9
#define KIND_FRACTAL7 10
#define KIND_FRACTAL8 11
#define KIND_FRACTAL9 12
#define KIND_FRACTAL10 13
#define KIND_FRACTAL11 14
uniform int kind;

// Time since the simulation started
uniform float time;

// What "palette" to use
uniform int palette_config;

// How much distortion to apply to the shapes
uniform float distortion;

// How fast to move the camera forwards
uniform float movement_speed;

float p_sin()
{
    return (sin(time) + 1.0) / 2.0;
}

// Colour pallete from https://www.shadertoy.com/view/ll2GD3
vec3 palette (float t) 
{
    vec3 a = vec3(0.5, 0.5, 0.5);    
    vec3 b = vec3(0.5, 0.5, 0.5);
    vec3 c = vec3(1.0, 1.0, 1.0);
    vec3 d = vec3(0.263, 0.416, 0.557);

    switch (palette_config) 
    {
        case 1:
            d = vec3(0.416, 0.234, 0.1);
            break;

        case 2:
            d = vec3(0.263, 0.416, 0.1);
            break;

        case 3:
            d = vec3(0.263, 0.1, 0.7);
            break;
    }
    return a + b * cos(6.28318 * (c * t + d));
}


// Standard 2D rotation using matrices
mat2 rotate_2d(float angle)
{
    return 
        mat2(
            cos(angle), -sin(angle),
            sin(angle), cos(angle)
        );
}

// Same as "fractal", just without the 4d vector
float fire_spiral(vec3 p) 
{
    for(int i = 0; i < 8; i++)
    {
        p.xyz = mod(p.xyz - 1.0, 2.0) - 1.0;
        p *= 1.4 / dot(p.xyz, p.xyz);
    }
    return (length(p.xz) * 0.25);
}

// All fractal and SDF functions are adapted from https://jbaker.graphics/writings/DEC.html 
// By gaziya5 aka gaz
float fractal1(vec3 p0) 
{   
    vec4 p = vec4(p0, 1.0);
    for(int i = 0; i < 8; i++)
    {
        p.xyz = mod(p.xyz - 1.0, 2.0) - 1.0;
        p *= 1.4 / dot(p.xyz, p.xyz);
    }
    return (length(p.xz / p.w ) * 0.25);
}

// By gaziya5 aka gaz
float fractal2(vec3 p) 
{
    p.xz = fract(p.xz) - 0.5;
    float k = 1.0;
    float s = 0.0;

    for (int i = 0; i < 8 ; i++) 
    {
        s = 2.0 / clamp(dot(p, p /* * (sin(time) + 1.5) / 2*/), 0.1, 1.0);
        p = abs(p) * s - vec3(0.5, 3.0, 0.5);
        k *= s;
    }
    return length(p) / k - 0.001;
}

// By kamoshika
float fractal3(vec3 p)
{
    float d = 1.0;
    vec3 q = fract(p) - 0.5;

    d = min(d, length(q.xy) - 0.13);
    d = min(d, length(q.yz) - 0.13);
    d = min(d, length(q.zx) - 0.13);

    d = max(d, 0.68 - length(fract(p - 0.5) - 0.5));
    return d;
}

// By gaziya5 aka gaz
float fractal4( vec3 p)
{
    float s = 3.0;
    for(int i = 0; i < 4; i++) 
    {
        p=mod(p-1.0, 2.0) - 1.0;
        float r=1.2 / dot(p,p);
        p*=r; 
        s*=r;
    }
    p = abs(p)  -0.8;
    if (p.x < p.z) p.xz = p.zx;
    if (p.y < p.z) p.yz = p.zy;
    if (p.x < p.y) p.xy = p.yx;
    return length(cross(p, normalize(vec3(0, 1, 1)))) / s - 0.001;
}

// By gaziya5 aka gaz
float fractal5( vec3 p)
{
    float itr=10.0;
    float r=0.1;
    p = mod(p-1.5, 3.0) - 1.5;
    p = abs(p) - 1.3;
    if (p.x < p.z) p.xz = p.zx;
    if (p.y < p.z) p.yz = p.zy;
    if (p.x < p.y) p.xy = p.yx;

    float s = 1.0;

    p -= vec3(0.5, -0.3, 1.5);
  	for(float i = 0.0; i++ < itr;) 
    {
  		float r2 = 2.0 / clamp(dot(p, p), 0.1, 1.0);
  		p = abs(p) * r2;
  		p -= vec3(0.7, 0.3, 5.5);
  		s *= r2;
  	}
    return length(p.xy) / (s-r);
}

// By gaziya5 aka gaz
float fractal6(vec3 p)
{
    float s = 2.0;
    float l = 0.0;
    p = abs(p);

    for(int j=0 ;j < 8; j++)
    {
        p = -sign(p) * (abs(abs(abs(p) - 2.0) - 1.0) - 1.0);
        p *= l = -1.3 / dot( p, p);
        p -= 0.15;
        s *= l;
    }
    return length(p) / s;
}

// By Kali
float fractal7(vec3 p)
{
    p.xz = abs(0.5 - mod(p.xz, 1.0)) + 0.01;
    float DEfactor = 1.0;
    for (int i=0; i<14; i++) 
    {
        p = abs(p) - vec3(0.0, 2.0, 0.0);
        float r2 = dot(p, p);
        float sc = 2.0 / clamp(r2 ,0.4, 1.0);
        p *= sc;
        DEfactor *= sc;
        p = p - vec3(0.5, 1.0, 0.5);
    }
    return length(p) / DEfactor - 0.0005;
}

	
// By gaziya5 aka gaz
float fractal8(vec3 p)
{
    p.xz = mod(p.xz, 2.0) - 1.0;
    float s = 2.0;
    float e;
    for(int j=0; j<8; j++) 
    {
      s *= e = 2.0 / clamp(dot(p, p), 0.5, 1.0);
      p = abs(p) * e - vec3(0.5, 8, 0.5);
    }
    return length(p.xz) / s;
}

#define PI 3.14159

// By yonatan
float fractal9(vec3 p)
{
    float y;
    float s;
    float e;
    p += vec3(1.0);
    y = p.y * 0.3 - 0.3;
    s  = 9.0;
    for (int j = 0; j++ < 9; p /= max(e, 0.0001)) 
    {
        p = mod(p - 1.0,2.0) - 1.0;
        p.zx *= rotate_2d (PI / 4.0);
        e = dot(p, p) * (0.6 + y);
        s /= max(e, 0.0001);
    }
    return e=sqrt(e) / s;
}

// by alia, adapted on https://jbaker.graphics/writings/DEC.html 
float fractal10(vec3 p)
{
    vec3 q = fract(p) - 0.5;

    float f = -length(p.xy) + 2.0;
    float g =  length(q)    - 0.6;
    return max(f,-g);

}

// By gaziya5 aka gaz
float fractal11(vec3 p){
    float e = 1.0;
    float B = 2.95;
    float H = 0.9; 
    float s = 2.0;

    p.z = mod(p.z-2.0, 4.0) - 2.0;

    for (int j = 0; j < 8; j++)
    {
      p=abs(p);
      p.x < p.z ? p = p.zyx : p;
      p.x=H-abs(p.x-H);
      p.y < p.z?p=p.xzy:p;
      p.xz+=.1;
      p.y < p.x?p=p.yxz:p;
      p.y-=.1;
    }
    p*=B; p-=2.5; s*=B;
    return length(p.xy)/s-.007;
}

float sd_torus(vec3 p, float r)
{
    return length(vec2(p.y, length(p.xz) - r));
}

float sd_sphere(vec3 p, float size)
{
    return length(p) - size;
 
}

float sd_box(vec3 p, vec3 size)
{
    vec3 d = abs(p) - size;
    return min(max(d.x, max(d.y, d.z)), 0.0) + length(max(d, 0.0));
}

// Rotate the vector around a circle based on time
vec3 circle_vec3(vec3 p, float divisor)
{
    p.y += sin(time) / divisor;
    p.x += cos(time) / divisor;
    return p;
}

float map_fracatals(in vec3 p) 
{
    if (kind == KIND_FRACTAL1) 
    {
        p = circle_vec3(p, 2);
        p.y -= 1;
        return fractal1(p);
    }
    else if (kind == KIND_FRACTAL2) 
    {
        p.y  -= 1;
        p.xy *= rotate_2d((time) / 8);
        return fractal2(p);
    }
    else if (kind == KIND_FRACTAL3) 
    {
        p.xy *= -rotate_2d((time) / 8);
        return fractal3(p);
    }
    else if (kind == KIND_FRACTAL4) 
    {
        p.x -= 0.5;
        p = -circle_vec3(p, 4);     
        p.xy *= -rotate_2d((time) / 8);

        return fractal4(p);
    }
    else if (kind == KIND_FRACTAL5) 
    {
        p.yz *= -rotate_2d((time) / 8);
        p = circle_vec3(p, 16);     
        return fractal5(p);
    }
    else if (kind == KIND_FRACTAL6) 
    {

        p.z += movement_speed * time;
        p.yz *= -rotate_2d((time) / 16);
        p = circle_vec3(p, 4);     
        return fractal6(p);
    }
    else if (kind == KIND_FRACTAL7) 
    {
        p.y += 1;   
        p.x += 1;      
        return fractal7(p);
    }
    else if (kind == KIND_FRACTAL8) 
    {
        p.y += 1;   
        p.x += 0.25;    
        p.xy *= -rotate_2d((time) / 16);   
        return fractal8(p);
    }
    else if (kind == KIND_FRACTAL9)
    {
        p.y  += 1;
        p.zx *= -rotate_2d((time) / -16);
        return fractal9(p);
    }
    else if (kind == KIND_FRACTAL10) 
    {
        p.xy *= -rotate_2d((time) / 8);
        p = circle_vec3(p, 4);  
        return fractal10(p);
    }
    else if (kind == KIND_FRACTAL11) 
    {
        p.y -= 1;
        p.xy *= -rotate_2d((time) / 8);
        p = circle_vec3(p, 16);  
        return fractal11(p);
    }

    else if (kind == KIND_FRACTAL_MIX) 
    {
        // Move the camera in a circle
        p = circle_vec3(p, 16);
        p.zx *= rotate_2d((time) / 32);
        p.y -= 0.5;

        return mix(fractal1(p), fractal2(p), (sin(time / 8) + 1) / 2);

        // return mix(mix(fractal4(p), fractal1(p), (cos(time / 8) + 1) / 2), fractal2(p), (sin(time / 8) + 1) / 2);
    }
    else if (kind == KIND_FRACTAL_MIX_1_AND_4) 
    {
        // Move the camera in a circle
        p.xy *= rotate_2d((time) / 8);
        p.x += 0.2;
        p.y += 0.4;

        return mix(fractal1(p), fractal4(p), cos(time / 8));
    }
}

float map_primatives(in vec3 p) 
{
    // Rotate the shapes

    // Create an infinite field
    // p.zx *= rotate_2d(time / 4);
    p.xy = fract(p.xy) - 0.5;
    p.z = mod(p.z, 0.5) - 0.25;

    if (kind == KIND_CUBES) 
    {
        p.zx *= rotate_2d(time / 4);
        //p.xy *= rotate_2d(time/4);
        return sd_box(p, vec3(0.1));
    }
    else if (kind == KIND_TORUS) 
    {
        return sd_torus(p, 0.25);
    }
}


void main() 
{
    // Set up initial data
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    vec2 image_size = imageSize(out_image);

    // Start in ndc sapce
    vec2 uv = (pixel_coords * 2.0 - image_size.xy) / image_size.y;
     
    // Ray origin point 
    vec3 ro = vec3(0, 1, -3);
    
    // Ray direction
    float fov = 1.5;
    vec3 rd = normalize(vec3(uv * fov, 1));
    
    // Begin the march
    float t = 0.1;
    int i = 0;
    for (i = 0; i < 80; i++) 
    {
        vec3 ray_position = ro + rd * t;

        // Spiral the ray as it gets further from the camera
        //ray_position.xy *= rotate_2d(t * distortion);
        ray_position.zy *= rotate_2d(t * distortion);

        // Continuously move the camera forwards
        ray_position.z += time * movement_speed;

        // Get the nearest signed-distance field distance
        float dist = 0;
        switch(kind)
        {
            case KIND_TORUS:
            case KIND_CUBES:
                dist = map_primatives(ray_position);
                break;

            case KIND_FRACTAL1:
            case KIND_FRACTAL2:
            case KIND_FRACTAL3:
            case KIND_FRACTAL4:
            case KIND_FRACTAL5:
            case KIND_FRACTAL6:
            case KIND_FRACTAL7:
            case KIND_FRACTAL8:
            case KIND_FRACTAL9:
            case KIND_FRACTAL10:
            case KIND_FRACTAL11:
            case KIND_FRACTAL_MIX:
            case KIND_FRACTAL_MIX_1_AND_4:
                dist = map_fracatals(ray_position);
                break;

        }

        // Step the travel based on the last sdf
        t += dist;

        // Exit early if the ray gets too close to the object, or if the travel distance is over 100
        if (dist < 0.001 || t > 150.0) 
        {
            break;
        }
    }


    vec3 colour = t < 99 ? palette(t * 0.05 + (float(i) / 80.0)) : vec3(0);

    imageStore(out_image, pixel_coords, vec4(colour, 1));

}