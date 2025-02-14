#version 430

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform float deltaTime;

// Output
out vec3 speed;
out vec3 position;
layout(location = 3) out float rotationAngle;

const vec3 control_p[8] = vec3[](
    vec3(-3.0, -1.0, 0.0),
    vec3(-1.0, 2.0, 0.0),
    vec3(1.0, -2.0, 0.0),
    vec3(3.0, 1.0, 0.0),
    vec3(2.0, 3.0, 0.0),
    vec3(0.0, 0.0, 0.0),
    vec3(-2.0, 3.0, 0.0),
    vec3(-3.0, -1.0, 0.0)
    );

struct Particle
{
    vec4 position, speed, iposition, ispeed;
    float delay, iDelay, lifetime, iLifetime;
    float initialRotationAngle;
};


layout(std430, binding = 0) buffer particles
{
    Particle data[];
};


float rand(vec2 co)
{
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}


void respawn(inout Particle particle, vec3 minBounds, vec3 maxBounds)
{
    particle.position.xyz = vec3(
        mix(minBounds.x, maxBounds.x, rand(vec2(particle.iLifetime, deltaTime))),
        maxBounds.y,
        mix(minBounds.z, maxBounds.z, rand(vec2(particle.iLifetime + 1.0, deltaTime)))
    );

    float speedFactor = 0.05 + rand(vec2(particle.iLifetime, deltaTime)) * 0.02;
    particle.speed.xyz = vec3(
        (rand(vec2(deltaTime, particle.iLifetime)) - 0.5) * 0.1,
        -speedFactor,
        (rand(vec2(deltaTime + 1.0, particle.iLifetime)) - 0.5) * 0.1
    );

    particle.delay = 0.5;
}


void main()
{
    vec3 minBounds = vec3(-15.0, 14.0, -15.0);
    vec3 maxBounds = vec3(15.0, 15.0, 15.0);

    vec3 pos = data[gl_VertexID].position.xyz;
    vec3 spd = data[gl_VertexID].speed.xyz;

    float delay = data[gl_VertexID].delay;
    delay -= deltaTime;

    if (delay > 0.0)
    {
        data[gl_VertexID].delay = delay;
        speed = spd;
        position = pos;
        gl_Position = Model * vec4(pos, 1.0);
        return;
    }

    pos += spd * deltaTime;

    if (pos.x < minBounds.x || pos.x > maxBounds.x ||
        pos.y < minBounds.y || pos.y > maxBounds.y ||
        pos.z < minBounds.z || pos.z > maxBounds.z)
    {
        respawn(data[gl_VertexID], minBounds, maxBounds);
        pos = data[gl_VertexID].position.xyz;
        spd = data[gl_VertexID].speed.xyz;
        delay = 0.5;
    }

    data[gl_VertexID].position.xyz = pos;
    data[gl_VertexID].speed.xyz = spd;
    data[gl_VertexID].delay = delay;

    position = pos;
    speed = spd;

    gl_Position = Model * vec4(pos, 1.0);
}
