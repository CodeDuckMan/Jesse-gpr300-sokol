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
// uniform float alpha; - moved to matierial
// uniform Material material;
uniform Palette pal;
uniform vec3 camera_position;


vec3 toonShading (vec3 normal, vec3 frag_position, vec3 light_pos) {


// replace the light_


vec3 view_dir = normalize(camera_position - frag_position);
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


float ndot1 = max

void main()
{
  vec3 lighting = blinnphong(vs_normal, vs_position, light);
  // vec3 lighting = blinnphong(vs_normal, vs_position, light) + ambient * 0.5;
  // vec3 object_color = vs_normal.rbg * 0.5 + 0.5;
  // vec3 ambient = vec3(1.0);
  // vce3 final_color = object_color * lighting;
  // FragColor = vec4(final_color, 1.0);
  FragColor = vec4(lighting, 1.0);

}