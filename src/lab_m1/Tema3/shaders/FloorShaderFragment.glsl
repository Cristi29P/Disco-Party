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

uniform float spot_angle;
uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

uniform vec3 pointLightDirection[4];
uniform vec3 pointLightPos[4];
uniform vec3 pointLightColor[4];

uniform vec3 emissive_component; 

// Switches
uniform float floorSwitch;
uniform float spotlightSwitch;
uniform float discoSwitch;

// Output
layout(location = 0) out vec4 out_color;

// Functii
vec3 DiscoGlobeContribution();

vec3 SpotlightContribution(vec3 lightPos, vec3 lightDirection, vec3 lightColor);

void main()
{
    vec3 spotlightIntensity = vec3(0.f, 0.f, 0.f);
    
    for (int i = 0; i < 4; ++i) {
        spotlightIntensity += SpotlightContribution(pointLightPos[i], pointLightDirection[i], pointLightColor[i]);
    }

     vec3 lightDisco = DiscoGlobeContribution();

    out_color = vec4(floorSwitch * emissive_component + spotlightSwitch * spotlightIntensity + discoSwitch * lightDisco, 1.f);
}

vec3 SpotlightContribution(vec3 lightPos, vec3 lightDirection, vec3 lightColor) {
    vec3 N = normalize(world_normal);
    vec3 L = normalize(lightPos - world_position);
    vec3 V = normalize(eye_position - world_position);
    vec3 H = normalize(L + V);

    float diffuse_light = material_kd * max(0.f,  dot(N, L));

    float specular_light = 0.f;

    if (diffuse_light > 0.f)
    {
        specular_light = material_ks * pow(max(0.f, dot(N, H)), material_shininess);
    }

    vec3 lightIntensity = vec3(0.f);

    float spot_light = dot(-L, lightDirection);
    float spot_light_limit = cos(spot_angle);

    if (spot_light > spot_light_limit)
    {
         float linear_att = (spot_light - spot_light_limit) / (1.0f - spot_light_limit);
         lightIntensity = lightColor * linear_att * (specular_light + diffuse_light);
    }

    return lightIntensity;
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