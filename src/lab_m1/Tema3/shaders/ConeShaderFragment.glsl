#version 330
// Input
uniform vec3 cone_color;

uniform float spotlightSwitch;

// Output
layout(location = 0) out vec4 out_color;



void main()
{
    if (spotlightSwitch != 0.f) {
         out_color = vec4(cone_color, 0.5f);
    } else {
         out_color = vec4(cone_color, 0.f);
    }
}
   
