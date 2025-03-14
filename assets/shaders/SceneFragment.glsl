#version 450 core

in vec2 pass_texture_coord;

uniform sampler2D image;

out vec4 out_colour;

void main() 
{
    out_colour = texture(image, pass_texture_coord);
}