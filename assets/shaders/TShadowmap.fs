#version 410

precision mediump float;

out vec4 FragColor;

struct Light {
  vec3 color;
  vec3 position;
};

struct Material{
  vec3 ambient;
  vec3 deffuse;
  vec3 specular;
  vec3 shinniness;
};

struct Palette {
vec3 color1;
vec3 color2;

}

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

// uniforms
uniform vec3 camera;
uniform vec3 light;
uniform vec3 light_color;
uniform alpha;
uniform Palette pal;
uniform vec3 camera_position;

float shadowCalculation (vec4 fragPositionLightSpace){
    float shadow = 0.25;

    vec3 proj_coords = fragPositionLightSpace.xyz / fragPositionLightSpace.w;
    proj_coords = proj_coords * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, proj_coords);
    
    float currentDepth = proj_coords.z;

    float shadow = 

    return shadow;
}

vec3 toonShading (vec3 normal, vec3 frag_position, vec3 light_pos) {


// replace the light_


vec3 view_dir = normalize(_p - frag_position);
vec3 light_dir = normalize(light_pos - frag_position);
vec3 half_dir =  normalize(light_dir + view_dir);

// dot products

float ndotl = (dot(normal, light_dir) + 1.0) * 0.5;
float ndoth = max(dot(normal, light_dir), 0.0);

float PdotL = dot(frag_position, light_pos.xyz);

return normalize(vec3(PdotL));
// return light_dir_ * light_color;
// return vce3(deffuse * light_color);

// deffuse = NdotL * material.deffuse;
// 
}



float ndot1 = max;

void main()
{
    vec3 normal = normalize(vs_normal);

    float shadow = shadowCalculation(vs_light_proj_pos);
    vec3 lightColor = toonShading(normal, vs_position, light.position, light.color);

    vec3 object_color = normal * 0.5 + 0.5;

    light_color += vec3(1.0,1.0,1.0);
    light_color *= light.color;

    FragColor = vec4(light_color * object_color, 1.0);

}
