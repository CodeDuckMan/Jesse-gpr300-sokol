#version 410

precision mediump float;

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;


uniform sampler2D screen;
uniform float strength = 16;

const float offset = 1.0 / 300.0;
const vec2 offsets[9] = vec2[]
(
  vec2(-offset, offset), // upleft
  vec2(0.0, offset), // upmid
  vec2(offset, offset), // upright
  
  vec2(-offset, 0.0), //midleft
  vec2(0.0, 0.0), //midmid
  vec2(offset, 0.0), //midright
  
  vec2(-offset, -offset), //downleft
  vec2(0.0, -offset), //downmid
  vec2(offset, -offset) //downright
);

const float kernal[9] = float[] (
  -1.0, -1.0, -1.0,
  -1.0, 9.0, -1.0,
  -1.0, -1.0, -1.0
);

void main()
{
  vec3 color = vec3(0,0,0);

  vec3 sampleTex[9];
  for(int i = 0; i < 9; i++)
  {
    sampleTex[i] = vec3(texture(screen, vs_texcoord.st + offsets[i]));
  }
  
  for(int i = 0; i < 9; i++)
  {
    color += sampleTex[i] * kernal[i];
  }
    
  //float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
  FragColor = vec4(color, 1.0);
  
}