#version 430

// Input
layout(location = 0) in vec2 tex_coord;
layout(location = 1) in vec3 p_speed;
layout(location = 2) in vec3 p_pos;
layout(location = 3) in float rot_angle;

// Uniform properties
uniform sampler2D texture_unit_0;
uniform float offset;
uniform float deltaTime;

// Output
layout(location = 0) out vec4 out_color;

const vec3 light_col = vec3(0.5, 1.0, 0.7);


mat2 rot_matrix(float angle)
{
    float c = cos(angle);
    float s = sin(angle);
    return mat2(c, -s, s, c);
}


float smooth_noise(vec2 coord)
{
    vec2 i = floor(coord);
    vec2 f = fract(coord);
    f = f * f * (3.0 - 2.0 * f);

    float a = fract(sin(dot(i, vec2(127.1, 311.7))) * 43758.5453123);
    float b = fract(sin(dot(i + vec2(1.0, 0.0), vec2(127.1, 311.7))) * 43758.5453123);
    float c = fract(sin(dot(i + vec2(0.0, 1.0), vec2(127.1, 311.7))) * 43758.5453123);
    float d = fract(sin(dot(i + vec2(1.0, 1.0), vec2(127.1, 311.7))) * 43758.5453123);

    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}


vec2 rand_offset(vec2 coord)
{
    float n = smooth_noise(coord);
    return vec2(n, smooth_noise(n + coord));
}


void main()
{
    vec2 rand_off = rand_offset(vec2(deltaTime, deltaTime * 2.0));
    vec3 jitter_pos = p_pos + vec3(rand_off, 0.0) * 0.1;

    float dist = length(jitter_pos - p_pos);
    float glow = offset / (dist * dist + 0.01);

    glow *= sin(deltaTime * 10.0 + p_speed.x) * 0.5 + 0.5;

    mat2 rot_mat = rot_matrix(rot_angle);
    vec2 rot_uv = rot_mat * (tex_coord - 0.5) + 0.5;

    vec3 tex = texture(texture_unit_0, rot_uv).rgb;
    vec3 color = glow * light_col * tex;

    out_color = vec4(color, 1.0);
}
