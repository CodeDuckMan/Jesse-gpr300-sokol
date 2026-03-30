#version 410

precision mediump float;

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;


uniform sampler2D screen;



void main()
{
  float Pixels = 512.0;
  float dx = 15.0 * (1.0 / Pixels);
  float dy = 10.0 * (1.0 / Pixels);
  vec2 Coord = vec2(dx * floor(vs_texcoord.x / dx),
                    dy * floor(vs_texcoord.y / dy));
  FragColor = texture(screen, Coord);
}