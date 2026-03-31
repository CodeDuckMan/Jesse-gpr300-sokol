#version 410

struct Light {
    vec3 color;
    vec3 position;
    float radius;
};

uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedo;
uniform sampler2D g_material;
uniform Light light;
uniform vec3 camera_position;

out vec4 FragLighting;

vec3 blinnPhon(vec3 position, vec3 normal, vec3 material) {

    return vec4(0.0,0.0,0.0,0.0;)
}

// varyings
in vec2 vs_texcoord;

void main()
{
    vec3 lighting = blinnPhong(position, normal, material);
    
    FragLighting = vec4(lighting, 1.0);
}