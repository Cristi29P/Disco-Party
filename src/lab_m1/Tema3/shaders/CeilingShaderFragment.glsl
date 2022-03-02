#version 330

// Input
in vec2 texcoord;
in vec3 world_position;
in vec3 world_normal;

// Texture coords
uniform sampler2D texture_1; 
uniform vec3 globe_position;
uniform float Time; // trebuie trimis acelasi time to be added

// Uniforms for light properties
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

// Output
layout(location = 0) out vec4 out_color;

// Switches

uniform float discoSwitch;

// Functii
vec3 DiscoGlobeContribution();

void main()
{
    vec3 lightDisco = DiscoGlobeContribution();

    out_color = vec4(discoSwitch * lightDisco, 1.0f);
}

vec3 DiscoGlobeContribution() {
    vec3 N = normalize(world_normal);
    vec3 L = normalize(globe_position - world_position);
    vec3 V = normalize(eye_position - world_position);
    vec3 H = normalize(L + V);
    
    float diffuse_light = material_kd * max(0.f,  dot(N, L));

    float specular_light = 0.f;

    if (diffuse_light > 0.f)
    {
        specular_light = material_ks * pow(max(0.f, dot(N, H)), material_shininess);
    }
    
    vec3 lightIntensity = vec3(0.f);
    
    float d = distance(world_position, globe_position);
    float atten = 1 / (0.2 + 0.4 * d + 0.06 * d * d);
    
    // CULOAREA
    vec3 light_dir = world_position - globe_position;
    vec2 texcoord;
    texcoord.x = (1.0 / (2 * 3.14159)) * atan (light_dir.x, light_dir.z) + 0.1f * Time;
    texcoord.y = (1.0 / 3.14159) * acos (light_dir.y / length (light_dir));
    vec3 color = texture2D(texture_1, texcoord).xyz;
    // CULOAREA

    lightIntensity = color * atten * (specular_light + diffuse_light);

    return lightIntensity;
}
