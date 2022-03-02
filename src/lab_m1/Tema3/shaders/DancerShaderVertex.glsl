#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// Output values to fragment shader
out vec2 texcoord;
out vec3 world_position;
out vec3 world_normal;

void main()
{
    texcoord = v_texture_coord;
    world_position = vec3(Model * vec4(v_position, 1.0));
    world_normal = vec3(Model * vec4(v_normal, 0.0));
    
    gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}

