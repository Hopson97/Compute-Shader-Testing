#version 460 core

#define MAX_RAY_LENGTH 100.0
#define MARCH 0.08
#define MAX_STEPS 80

layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform writeonly image2D out_image;

uniform mat4 inv_projection;
uniform mat4 inv_view;
uniform vec3 position;

struct Ray 
{
	vec3 origin;
	vec3 direction;
};

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

float map(in vec3 p) 
{
    float box = sd_box(p - vec3(5.0, 2, 5.0), vec3(1.0));
    
    return -box;
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

	Ray ray = Ray(position, ray_direction);

    vec4 final_colour = vec4(0.0);
    
    // As an optimization, only do the raymarch if the 
    vec2 cube = interesect_cube(ray, vec3(4.0, 1.0, 4.0), vec3(2.0));
    if (cube.x < cube.y)
    {
        float t = cube.x;
        for (int i = 0; i < MAX_STEPS; i++) 
        {
            vec3 p = ray.origin + ray.direction * t;
        
            float density = map(p);
        
            t += MARCH;
        
            if (density > 0.0)
            {
                vec4 colour = vec4(mix(vec3(1.0,1.0,1.0), vec3(0.0, 0.0, 0.0), density), density );
                colour.rgb *= colour.a;
                final_colour += colour * (1.0 - final_colour.a);
            }
            else if (t > MAX_RAY_LENGTH) 
            {
                break;
            }
        }
    }
    else 
    {
      //  final_colour = vec4(0.5, 0.6, 0.8, 1);
    }

    imageStore(out_image, pixel_coords, vec4(final_colour.rgb, 1.0));

}