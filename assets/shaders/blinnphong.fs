#version 410

precision mediump float;

out vec4 FragColor;

struct Light {
  vec3 color;
  vec3 position;
};

struct Ambient {
  vec3 color;
  vec3 position;
};

struct MaterialProperties {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shinniness;
};

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

// uniforms
uniform sampler2D texture0;
uniform vec3 camera_position;
uniform vec3 lightPosition;
uniform vec3 light_color;

uniform Light lightStruct;

// uniform float alpha; - moved to matierial
// uniform Material material;


vec3 blinnphong (vec3 normal, vec3 frag_position, vec3 light_pos) {

  vec3 textureColor = texture(texture0, vs_texcoord).rgb;

  // Ambient
  vec3 ambient = 0.05 * textureColor;

  // Diffuse
  vec3 light_dir = normalize(light_pos - frag_position);
  vec3 difNormal = normalize(normal);

  float diff = max(dot(light_dir, difNormal), 0.0);
  vec3 diffuse = diff * textureColor;

  // Specular
  // Normalise inputs
  vec3 view_dir = normalize(camera_position - frag_position);
  vec3 reflect_dir = reflect(light_dir, vs_normal);

  // Blinnphong part
  vec3 half_dir = normalize(light_dir + view_dir);
  float spec = pow(max(dot(normal, half_dir), 0.0), 32.0);

  vec3 specular = vec3(0.3) * spec;
  return (ambient + diffuse + specular);

}


void main()
{
  vec3 lighting = blinnphong(vs_normal, vs_position, lightPosition);
  // vec3 lighting = blinnphong(vs_normal, vs_position, light) + ambient * 0.5;
  // vec3 object_color = vs_normal.rbg * 0.5 + 0.5;
  // vec3 ambient = vec3(1.0);
  // vce3 final_color = object_color * lighting;
  // FragColor = vec4(final_color, 1.0);
  FragColor = vec4(lighting, 1.0);

}