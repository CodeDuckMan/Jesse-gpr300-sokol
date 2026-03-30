#version 410

precision mediump float;

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;

uniform sampler2D screen;
uniform float time;

uniform float offsetX;
uniform float offsetY;
uniform float offsetZ;
uniform float rDirectionX;
uniform float rDirectionY;
uniform float gDirectionX;
uniform float gDirectionY;
uniform float bDirectionX;
uniform float bDirectionY;

void main()
{
  vec3 color;

  vec3 offset = vec3(offsetX, offsetY, offsetZ);
  vec2 rDirection = vec2(rDirectionX, rDirectionY);
  vec2 gDirection = vec2(gDirectionX, gDirectionY);
  vec2 bDirection = vec2(bDirectionX, bDirectionY);

  float gate = 0.5 + 0.5 * sin(time * 2.3 + sin(time * 0.7));

  color.r = texture(screen, vs_texcoord + (sin(time) * rDirection * vec2(offset.r))).r;
  color.g = texture(screen, vs_texcoord + (sin(time) * gDirection * vec2(offset.g))).g;
  color.b = texture(screen, vs_texcoord + (sin(time) * bDirection * vec2(offset.b))).b;
  
  FragColor = vec4(color, 1.0);
}