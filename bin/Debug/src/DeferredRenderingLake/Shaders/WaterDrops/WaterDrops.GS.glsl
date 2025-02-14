#version 430

// Input and output topologies
layout(points) in;
layout(triangle_strip, max_vertices = 6) out;

// Uniform properties
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 eye_position;

uniform float offset;

in float vert_lifetime[1];
in float vert_iLifetime[1];

// Output
layout(location = 0) out vec2 texture_coord;
layout(location = 1) out float geom_lifetime;
layout(location = 2) out float geom_iLifetime;

// Vertex properties
vec3 vpos = gl_in[0].gl_Position.xyz;

// Direction vectors
vec3 forward = normalize(eye_position - vpos);
vec3 right = normalize(cross(forward, vec3(0, 1, 0)));
vec3 up = normalize(cross(forward, right));


void EmitPoint(vec2 offset, vec2 text_coord)
{
    // Emit vertex
    vec3 pos = vpos + right * offset.x + up * offset.y;
    gl_Position = Projection * View * vec4(pos, 1.0);

    texture_coord = text_coord;
    geom_lifetime = vert_lifetime[0];
    geom_iLifetime = vert_iLifetime[0];
  
    EmitVertex();
}


void main()
{
    float ds = offset;
    EmitPoint(vec2(ds, -ds), vec2(0, 0));
    EmitPoint(vec2(-ds, -ds), vec2(1, 0));
    EmitPoint(vec2(ds, ds), vec2(0, 1));
    EmitPoint(vec2(-ds, ds), vec2(1, 1));
    EndPrimitive();
}