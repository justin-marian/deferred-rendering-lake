#version 430

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// Outputs
layout(location = 0) out vec3 world_position;
layout(location = 1) out vec3 world_normal;
layout(location = 2) out vec3 world_view;

void main()
{
    world_position = (Model * vec4(v_position, 1.0)).xyz;
    world_normal = normalize(mat3(transpose(inverse(Model))) * v_normal);
    world_view = normalize(-world_position);

    gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}
