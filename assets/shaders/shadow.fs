#version 300 es

precision mediump float;

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};
struct Ambient {
  float intensity;
  vec3 color;
};
struct Light {
  vec3 color;
  vec3 position;
};

out vec4 FragColor;

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;
in vec4 vs_light_proj_pos;

// uniforms
uniform sampler2D shadow_map;
uniform vec3 camera_position;
uniform Material material;
uniform Ambient ambient;
uniform Light light;
uniform float bias;
uniform bool use_pcf;


uniform vec3 lightPosition;
uniform vec3 light_color;

uniform Light lightStruct;


float shadowCalculation(vec4 fragPosLightSpace)
{
    // Perspective devide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // Get depth values 
    float closestDepth = texture(shadow_map, projCoords.xy).r;
    float currentDepth = projCoords.z;

    // Check frag pos 
    vec3 normal = normalize(vs_normal);
    vec3 lightDir = normalize(light.position - vs_position);
    // float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = (currentDepth - bias) > closestDepth ? 1.0 : 0.0;

    if (use_pcf)
    {
      shadow = 0.0;
      vec2 texel_size = 1.0 / vec2(1024.0, 1024.0);
      for(int x = -1; x <= 1; ++x)
      {
          for(int y = -1; y <= 1; ++y)
          {
              float pcf_depth = texture(shadow_map, projCoords.xy + vec2(x, y) * texel_size).r; 
              shadow += (currentDepth - bias) > pcf_depth  ? 1.0 : 0.0;        
          }    
      }
      shadow /= 9.0;
    }

    return shadow;
}


vec3 blinnPhong(vec3 normal, vec3 frag_pos, vec3 light_pos) {

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
  vec3 normal = normalize(vs_normal);
  vec3 objectColor = (normal * 0.5 + 0.5);

  float shadow = shadowCalculation(vs_light_proj_pos);

  vec3 lighting = blinnPhong(normal, vs_position, light.position);
  lighting *= (1.0 - shadow);
  lighting += ambient.color * material.ambient;
  lighting *= light.color;

  FragColor = vec4(objectColor * lighting, 1.0);
}