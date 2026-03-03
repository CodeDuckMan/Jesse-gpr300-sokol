#version 410

precision mediump float;

out vec4 FragColor;

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

// uniforms
uniform sampler2D texture0;
uniform vec3 cameraPosition;
uniform float time;
uniform vec3 waterColor;



void main()
{
  
  vec2 dir = vec2(1.0, 0.0);
  vec2 uv = vs_texcoord + vec2(time * dir);
  uv.x += sin(uv.x * 3.5 + time);
  uv.y += sin(uv.y * 3.5 + time);

  vec4 sample1 = texture(texture0, uv * 1.0);
  vec4 sample2 = texture(texture0, uv * 1.2);

  vec3 object_color = vec3(sample1 * 0.75) - vec3(sample2 * 0.25);

  FragColor = vec4(waterColor + object_color, 1.0);

}