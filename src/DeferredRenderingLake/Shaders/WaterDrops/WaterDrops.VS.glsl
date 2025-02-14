#version 430

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Output
uniform mat4 Model;
uniform float deltaTime;
uniform vec3 control_p0;
uniform vec3 control_p1;
uniform vec3 control_p2;
uniform vec3 control_p3;
uniform float displacement_at_p0;

const float g = 9.81;


struct Particle 
{
    vec4 position, speed, iposition, ispeed;
    float delay, iDelay, lifetime, iLifetime;
};


layout(std430, binding = 0) buffer particles 
{
    Particle data[];
};


float rand(vec2 co)
{
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}


vec3 bezier(float t)
{
    return
        pow(1.0 - t, 3.0) * control_p0 +
        3.0 * t * pow(1.0 - t, 2.0) * control_p1 +
        3.0 * pow(t, 2.0) * (1.0 - t) * control_p2 +
        pow(t, 3.0) * control_p3;
}


vec3 bezier_derivative(float t) 
{
    return
        -3.0 * pow(1.0 - t, 2.0) * control_p0 +
        (3.0 * pow(1.0 - t, 2.0) - 6.0 * t * (1.0 - t)) * control_p1 +
        (6.0 * t * (1.0 - t) - 3.0 * pow(t, 2.0)) * control_p2 +
        3.0 * pow(t, 2.0) * control_p3;
}


void main() 
{
    vec3 pos = data[gl_VertexID].position.xyz;
    vec3 spd = data[gl_VertexID].speed.xyz;

    float delay = data[gl_VertexID].delay;
    delay -= deltaTime;

    if (delay > 0.0) 
    {
        data[gl_VertexID].delay = delay;
        gl_Position = Model * vec4(pos, 1.0);
        return;
    }

    float t = mod(float(gl_VertexID) / float(data.length()), 1.0);
    vec3 bezierPos = bezier(t);
    bezierPos.y += displacement_at_p0;

    float spread_factor = 0.5;
    bezierPos.x += (rand(vec2(pos.x, pos.z)) * 2.0 - 1.0) * spread_factor;
    bezierPos.z += (rand(vec2(pos.z, pos.x)) * 2.0 - 1.0) * spread_factor;

    spd = bezier_derivative(t);
    spd.y -= g * deltaTime * 0.5;
    pos += spd * deltaTime * 0.1;

    if (pos.y < - 2.5) 
    {
        float t = rand(vec2(gl_VertexID, deltaTime));
        vec3 base_pos = bezier(t);
        base_pos.y += displacement_at_p0;

        float spread_factor = 1.0; // Widen the particle range
        base_pos.x += (rand(vec2(base_pos.x, base_pos.z)) * 2.0 - 1.0) * spread_factor;
        base_pos.z += (rand(vec2(base_pos.z, base_pos.x)) * 2.0 - 1.0) * spread_factor;

        vec3 start_speed = normalize(bezier_derivative(t)) * 0.01;
        start_speed.y -= g * 0.01;

        data[gl_VertexID].position.xyz = base_pos;
        data[gl_VertexID].speed.xyz = start_speed;
        data[gl_VertexID].delay = rand(vec2(gl_VertexID, deltaTime)) * 0.5 + 0.1;

        pos = data[gl_VertexID].position.xyz;
        spd = data[gl_VertexID].speed.xyz;
    }

    data[gl_VertexID].position.xyz = pos;
    data[gl_VertexID].speed.xyz = spd;
    data[gl_VertexID].delay = delay;

    gl_Position = Model * vec4(pos, 1.0);
}
