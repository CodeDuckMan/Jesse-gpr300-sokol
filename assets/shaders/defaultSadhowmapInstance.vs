#version 300 es

// attributes
layout(location = 0) in vec2 in_position;
layout(location = 1) in vec2 in_texcoord;
layout(location = 2) in vec2 in_normal;
layout(location = 3) in vec4 in_instancedMatrix;


// varyings
out vec2 vs_texcoord;
out vec3 vs_position;

void main()
{
  vs_position = vec3(in_instance)
  vs_texcoord = in_texcoord;
  gl_Position = vec4(in_position.xy, 0.0, 1.0);
}