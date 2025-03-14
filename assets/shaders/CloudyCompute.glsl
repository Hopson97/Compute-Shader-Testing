#version 460 core

layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform writeonly image2D out_image;

uniform mat4 inv_projection;
uniform mat4 inv_view;
uniform vec3 position;
uniform float time;

#define MAX_RAY_LENGTH 100.0
#define MARCH_SIZE 0.1
#define MAX_STEPS 80

mat2 rotate_2d(float angle)
{
    float c = cos(angle);
    float s = sin(angle);
    return 
        mat2(
            c, -s,
            s, c
        );
}

struct Ray 
{
	vec3 origin;
	vec3 direction;
};

// Noise functions from https://www.shadertoy.com/view/lss3zr

/////////////////////////////////////
const mat3 m = mat3(0.00, 0.80, 0.60,
  -0.80, 0.36, -0.48,
  -0.60, -0.48, 0.64);
float hash( float n )
{
    return fract(sin(n)*43758.5453);
}

float noise(in vec3 x)
{
    vec3 p = floor(x);
    vec3 f = fract(x);

    f = f*f*(3.0-2.0*f);

    float n = p.x + p.y*57.0 + 113.0*p.z;

    float res = mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
                        mix( hash(n+ 57.0), hash(n+ 58.0),f.x),f.y),
                    mix(mix( hash(n+113.0), hash(n+114.0),f.x),
                        mix( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
    return res;
}

float fbm( vec3 p )
{
    p.xz *= rotate_2d(time / 4.0f);
    float f;
    f = 0.5000 * noise(p); p = m * p * 2.02;
    f += 0.2500 * noise(p); p = m * p * 2.03;
    f += 0.12500 * noise(p); p = m * p * 2.01;
    f += 0.06250 * noise(p);
    return f;
}
/////////////////////////////////////
/*

float noise(vec3 x ) {
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);

  vec2 uv = (p.xy+vec2(37.0,239.0)*p.z) + f.xy;
  vec2 tex = textureLod(noise_image,(uv+0.5)/256.0,0.0).yx;

    return mix( tex.x, tex.y, f.z ) * 2.0 - 1.0;
}

float fbm(vec3 p) {
  vec3 q = p + time * 0.5 * vec3(1.0, -0.2, -1.0);
  float g = noise(q);

  float f = 0.0;
  float scale = 0.5;
  float factor = 2.02;

  for (int i = 0; i < 6; i++) {
      f += scale * noise(q);
      q *= factor;
      factor += 0.21;
      scale *= 0.5;
  }

  return f;
}


*/
/// Ray/Line and Cube intersection test
/// Returns a vec2 where 
///         x is the distance where the ray ENTERS the cube
///         y is the distance where the ray EXITS the cube
vec2 interesect_cube(Ray ray, vec3 cube_min, vec3 cube_size) 
{
    // Find minimum point at which the ray enters the cube
    vec3 t_min = (cube_min - ray.origin) / ray.direction;

    // Find maximum point at which the ray exits the cube
    vec3 t_max = ((cube_min + cube_size) - ray.origin) / ray.direction;
    
    // Ensure the entry and exit are the "correct" way around
    vec3 entry = min(t_min, t_max);
    vec3 exit = max(t_min, t_max);
    
    // Get the distance to the box entry and exit
    float near = max(max(entry.x, entry.y), entry.z);
    float far = min(min(exit.x, exit.y), exit.z);
    
    return vec2(near, far);
}


float sd_spehere(vec3 p, float radius) 
{
    return length(p) - radius;
}

float sd_box(vec3 p, vec3 size)
{
    vec3 d = abs(p) - size;
    return min(max(d.x, max(d.y, d.z)), 0.0) + length(max(d, 0.0));
}

float sd_torus(vec3 p, vec2 t)
{
  vec2 q = vec2(length(p.xz)-t.x,p.y);
  return min(length(q/2)-t.y/2, length(q)-t.y);
}

float map(in vec3 p) 
{
   // float d = sd_box(p - vec3(5.0, 5.0, 5.0), vec3(3.0));
    float d = sd_spehere(p - vec3(4.5, 4.5, 4.5), 4.0);
    // d = sd_torus(p- vec3(4.5, 4.5, 4.5), vec2(3.0, 1.0));
    
    return -d + fbm(p) / 2.0;
}


vec4 march(vec3 ray_direction)
{
	Ray ray = Ray(position, ray_direction);

    // Output colour
    vec4 final_colour = vec4(0.0);
    
    // As an optimisation, only do the raymarch if the ray intersects the volume
    //vec2 cube = interesect_cube(ray, vec3(0.0, 0.0,0.0), vec3(10.0));
    //if (cube.x < cube.y)
    {

        //final_colour = vec4(0.1, 0.0, 0.0, 0.0);
        // Begin marching the volume where the intersection takes place
        //float t = cube.x;
        float t = 0.1;
        for (int i = 0; i < MAX_STEPS; i++) 
        {
            vec3 p = ray.origin + ray.direction * t;
            
            // Get the density from the sdf functions
            float density = map(p);
            
            // Step the ray a small amount each time rather than the SDF distance
            t += MARCH_SIZE;
            
            // Density is only greater than 0 INSIDE the volume
            if (density > 0.0)
            {
                vec4 colour = vec4(mix(vec3(1.0,1.0,1.0), vec3(0.0, 0.0, 0.0), density), density );
                colour.rgb *= colour.a;
                final_colour += colour * (1.0 - final_colour.a);
            }
            else if (t > MAX_RAY_LENGTH )//+ cube.x || t > cube.y) 
            {
                break;
            }
        }
    }
    return final_colour;
}


void main() 
{
	// Set up initial data
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	vec2 image_size = imageSize(out_image);
	vec2 ndc = ((pixel_coords / image_size)) * 2.0 - 1.0;
	vec4 eye = vec4(vec2(inv_projection * vec4(ndc, -1.0, 1.0)), -1.0, 0.0);
	vec4 world = inv_view * eye;
	vec3 ray_direction = normalize(world).xyz;


    vec4 final_colour = march(ray_direction);

    imageStore(out_image, pixel_coords, vec4(final_colour.rgb, 1.0));

}