#version 410

out vec4 FragColor;

// varyings
in vec2 vs_texcord;

// uniform
uniform sampler2D screen;

void main()
{
    vec3 color = texture(screen, vs_texcord).rgb;
    FragColor = vec4(color, 1.0);
}
