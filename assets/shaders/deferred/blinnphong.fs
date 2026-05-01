#version 410

// Structs
struct Light {
    vec3 color;
    vec3 position;
    float radius;
};

struct Ambient {
    vec3 color;
    vec3 position;
};

precision mediump float;
precision mediump int;
layout(location = 0) out vec4 frag_lighting;

// Uniforms
uniform vec2 textureSize; 
uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedo;
uniform sampler2D g_material;
uniform Light light;
uniform vec3 camera_position;

// Out
out vec4 FragLighting;

// Varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

// Methods


float calculateAttentuation(float dist, float radius)
{
	float i = clamp(1.0 - pow(dist/radius,4.0), 0.0, 1.0);
	return i * i;
}

vec3 blinnPhong(vec3 position, vec3 normal, vec4 material) {
    
    // Dirs
    vec3 viewDir = normalize(camera_position - position);
    vec3 lightDir = normalize(light.position - position);
    vec3 halfwayDir = normalize(lightDir + viewDir);  

    // dot products
    float ndotl = max(dot(normal, lightDir), 0.0);
    float ndoth = max(dot(normal, halfwayDir), 0.0);

    // components
    vec3 diffuse = ndotl * vec3(material.g);
    vec3 specular = pow(ndoth, material.a * 128.0) * vec3(material.b); 

    float attentuation = calculateAttentuation(length(light.position - position), light.radius);
    return (diffuse + specular) * attentuation * light.color;

    // float roughness = material.r;
    // float metallic = material.g;
    // float ao = material.b;

    // vec3 ambient = vec3(0.03) * albedo * ao;

    // float diff = max(dot(normal, lightDir), 0.0);
    // vec3 diffuse = diff * albedo * light.color;

    // float shininess = (1.0 - roughness) * 128.0;
    // float spec = pow(max(dot(normal, halfVec), 0.0), shininess);
    // vec3 specular = spec * mix(vec3(0.04), albedo, metallic) * light.color;

    // float dist = length(light.position - position);
    // float attenuation = 1.0 / (1.0 + (dist * dist) / (light.radius * light.radius));

    // return ambient + (diffuse + specular) * attenuation;
}

void main()
{
  vec2 uv = gl_FragCoord.xy / textureSize;

  vec3 position = texture(g_position, uv).xyz;
  vec3 normal = texture(g_normal, uv).xyz;
  vec3 albedo = texture(g_albedo, uv).xyz;
  vec4 material = texture(g_material, uv).rgba;

  vec3 lighting = blinnPhong(position, normal, material);
  frag_lighting = vec4(lighting, 1.0);
}