#version 430

layout(location = 0) in vec3 world_position;
layout(location = 1) in vec3 world_normal;
layout(location = 2) in vec2 texture_coords;

uniform samplerCube texture_cubemap;
uniform sampler2D texture_light;

uniform vec3 camera_position;
uniform mat4 view_matrix;

layout(location = 0) out vec4 out_color;


vec3 reflectionS(vec3 DIR)
{
    vec3 cubemap_col = texture(texture_cubemap, normalize(DIR)).rgb;
    vec2 uv_map = clamp(texture_coords, 0.0, 1.0);
    vec3 light_col = texture(texture_light, uv_map).rgb; // something arround the lake shape circle like
    return cubemap_col + light_col;
}


vec3 myReflect()
{
    vec3 V = normalize(world_position - camera_position);
    vec3 R = reflect(V, normalize(world_normal));
    mat3 VT = mat3(transpose(view_matrix));
    vec3 RT = VT * R;
    return reflectionS(RT);
}


void main()
{
    vec3 color = myReflect();
    out_color = vec4(clamp(color, 0.0, 1.0), 1.0);
}
