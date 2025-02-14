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


const vec3 control_p[8] = vec3[]
(
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
        mix(minBounds.y, maxBounds.y, rand(vec2(particle.iLifetime + 1.0, deltaTime))),
        mix(minBounds.z, maxBounds.z, rand(vec2(particle.iLifetime + 2.0, deltaTime)))
    );

    particle.speed.xyz = vec3(
        (rand(vec2(deltaTime, particle.iLifetime)) - 0.5) * 0.2,
        (rand(vec2(particle.iLifetime + 1.0, deltaTime)) - 0.5) * 0.2,
        (rand(vec2(particle.iLifetime + 2.0, deltaTime)) - 0.5) * 0.2
    );

    particle.delay = 0.25;
    particle.initialRotationAngle += radians(180.0);
}


vec3 b_spline(vec3 control_p0, vec3 control_p1, vec3 control_p2, vec3 control_p3, float t) 
{
    float u = t;
    float uu = u * u;
    float uuu = uu * u;

    return (1.0 / 6.0) * (
        (-uuu + 3.0 * uu - 3.0 * u + 1.0) * control_p0 +
        (3.0 * uuu - 6.0 * uu + 4.0) * control_p1 +
        (-3.0 * uuu + 3.0 * uu + 3.0 * u + 1.0) * control_p2 +
        (uuu)*control_p3
        );
}


vec3 control_points(vec3 control_p[8], int i) 
{
    int points = 8;
    return control_p[(i + points) % points];
}


void main() 
{
    vec3 minBounds = vec3(-15.0, 1.0, -15.0);
    vec3 maxBounds = vec3(15.0, 4.0, 15.0);

    vec3 pos = data[gl_VertexID].position.xyz;
    vec3 spd = data[gl_VertexID].speed.xyz;

    float delay = data[gl_VertexID].delay;
    delay -= deltaTime;

    if (delay > 0.0) 
    {
        data[gl_VertexID].delay = delay;
        speed = spd;
        position = pos;
        rotationAngle = data[gl_VertexID].initialRotationAngle;
        gl_Position = Model * vec4(pos, 1.0);
        return;
    }

    float time = data[gl_VertexID].iLifetime - delay;
    float t = fract(time * 0.05);

    int index = int(t * 4.0);
    vec3 control_p0 = control_points(control_p, index - 1);
    vec3 control_p1 = control_points(control_p, index);
    vec3 control_p2 = control_points(control_p, index + 1);
    vec3 control_p3 = control_points(control_p, index + 2);

    float localT = fract(t * 4.0);
    vec3 pos_spline = b_spline(control_p0, control_p1, control_p2, control_p3, localT);

    float oscillation = sin(deltaTime * 4.0 + spd.y * 3.0) * 0.5;
    pos += (pos_spline + vec3(0.0, oscillation, 0.0)) * spd * deltaTime * 2.0;

    spd.x += (rand(pos.xy) - 0.5) * 0.005;
    spd.y += (rand(pos.yz) - 0.5) * 0.005;
    spd.z += (rand(pos.zx) - 0.5) * 0.005;
    spd = clamp(spd, vec3(-0.4), vec3(0.4));

    if (pos.x < minBounds.x || pos.x > maxBounds.x ||
        pos.y < minBounds.y || pos.y > maxBounds.y ||
        pos.z < minBounds.z || pos.z > maxBounds.z) 
    {
        respawn(data[gl_VertexID], minBounds, maxBounds);
        pos = data[gl_VertexID].position.xyz;
        spd = data[gl_VertexID].speed.xyz;
        delay = data[gl_VertexID].delay;
    }

    data[gl_VertexID].position.xyz = pos;
    data[gl_VertexID].speed.xyz = spd;
    data[gl_VertexID].delay = delay;

    position = pos;
    speed = spd;
    rotationAngle = data[gl_VertexID].initialRotationAngle;

    gl_Position = Model * vec4(pos, 1.0);
}
