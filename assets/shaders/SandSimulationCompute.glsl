#version 460 core

#define COLOUR_THRESHOLD 0.05

layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform readonly image2D in_screen;
layout(rgba32f, binding = 1) uniform writeonly image2D out_screen;

uniform float brush_size;
uniform bool mouse_down;
uniform ivec2 mouse_coords;
uniform int generation;

const vec4 EMPTY = vec4(0, 0, 0, 0);


layout(std430, binding = 0) buffer ClaimsBufer
{
    uint claims[];
};

bool colour_is_same(vec4 left, vec4 right) 
{
    return  
        abs(left.r - right.r) < COLOUR_THRESHOLD &&
        abs(left.g - right.g) < COLOUR_THRESHOLD &&
        abs(left.b - right.b) < COLOUR_THRESHOLD;
}

bool check_atomic_claim(ivec2 pixel_coord, int width)
{
    int index = pixel_coord.y * width + pixel_coord.x;

    uint previous = atomicCompSwap(claims[index], 0u, 1u);

    return previous == 0u;
}



void main()
{
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    ivec2 image_size = imageSize(in_screen);
    int width = image_size.x;

    if (pixel_coords.x >= image_size.x || pixel_coords.y >= image_size.y || pixel_coords.x < 0 || pixel_coords.y < 0)
    {
        return;
    }

    vec4 current = imageLoad(in_screen, pixel_coords);


    // Do the simulation
    if (pixel_coords.y > 100 && !colour_is_same(current, EMPTY))
    {
        bool odd = generation % 2 == 1;
        ivec2 below = ivec2(pixel_coords.x, pixel_coords.y - 1);
        ivec2 direction_a = ivec2(pixel_coords.x + (odd ? 1 : -1), pixel_coords.y - 1);
        ivec2 direction_b = ivec2(pixel_coords.x - (odd ? 1 : -1), pixel_coords.y - 1);

        // Check the below pixel is empty, and "move" the pixel
        if (colour_is_same(imageLoad(in_screen, below), EMPTY) && check_atomic_claim(below, width))
        {
            imageStore(out_screen, pixel_coords, EMPTY);
            imageStore(out_screen, below, current);
        }
        else if (colour_is_same(imageLoad(in_screen, direction_a), EMPTY) && check_atomic_claim(direction_a, width))
        {
            imageStore(out_screen, pixel_coords, EMPTY);
            imageStore(out_screen, direction_a, current);
        }
        else if (colour_is_same(imageLoad(in_screen, direction_b), EMPTY) && check_atomic_claim(direction_b, width))
        {
            imageStore(out_screen, pixel_coords, EMPTY);
            imageStore(out_screen, direction_b, current);
        }
        else 
        {

            imageStore(out_screen, pixel_coords, current);
        }
    }
    else 
    {
        imageStore(out_screen, pixel_coords, current);
    }


    // Check for mouse input
    if (mouse_down && distance(pixel_coords, mouse_coords) < brush_size && colour_is_same(current, EMPTY))
    {
        float g = float(pixel_coords.x) / float(max(1, image_size.x));
        float b = float(pixel_coords.y) / float(max(1, image_size.y));
        imageStore(out_screen, pixel_coords, vec4(0, g, b, 1));
    }
}