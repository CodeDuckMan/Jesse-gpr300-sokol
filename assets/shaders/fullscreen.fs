#version 410

// varyings
in vec2 vs_texcord;

// uniforms

uniform sampler2D screen;

void main()
{
    vec3 color = texture(screen, vs_texcord).rgb;
    fragColor = vec4(color, 1.0);
}
