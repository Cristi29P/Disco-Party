#version 330

// Input
in vec2 texcoord;
in vec3 world_position;

// Uniform properties
uniform sampler2D texture_1;

uniform vec3 globe_position;

uniform float Time;

// Switches
uniform float discoSwitch;

// Output
layout(location = 0) out vec4 out_color;



void main()
{
    vec3 light_dir = world_position - globe_position;

    vec2 texcoord;

    texcoord.x = (1.0 / (2 * 3.14159)) * atan (light_dir.x, light_dir.z) + 0.1f * Time;
    texcoord.y = (1.0 / 3.14159) * acos (light_dir.y / length (light_dir));

    vec3 color = texture2D(texture_1, texcoord).xyz;

    out_color = vec4(discoSwitch * color, 1.f);
}
