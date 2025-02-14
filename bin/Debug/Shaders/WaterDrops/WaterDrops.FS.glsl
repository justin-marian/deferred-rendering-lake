#version 430

// Input
layout(location = 0) in vec2 texture_coord;

// Uniform properties
uniform sampler2D u_texture_0;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    vec3 color = texture(u_texture_0, texture_coord).xyz;
    out_color = vec4(color, 1);
}
