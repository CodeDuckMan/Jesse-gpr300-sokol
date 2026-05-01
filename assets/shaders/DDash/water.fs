#version 410

precision mediump float;

out vec4 FragColor;

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

// uniforms
uniform sampler2D wave_tex;
uniform sampler2D wave_spec;
uniform sampler2D wave_warp;

uniform vec3 camera_position;

uniform float time;
uniform vec3 waterColor;

// Changes the number of tiles - higher value = more scales
float scale = 1;

void main()
{
  // Warp
  vec2 warp_uv = vs_texcoord * scale;
  vec2 warpScroll = vec2(0.5, 0.5_ * dTime);
  vec2 warp = texture(wave_warp, warp_uv + warpScroll).xy;

  // Albedo = base color
  vec2 albedo_uv = vs_texcoord * scale;
  vec3 albedo = texture(wave_tex, albedo_uv + warp).rgb;

  vec3 finalColor = waterColor + vec3(albedo.a);
  // Specular - shimmer
  vec4 specSample1 = texture(wave_spec, spec_uv + vec2(0.5, 0.5) * time).rgb;
  vec4 specSample2 = texture(wave_spec, spec_uv + vec2(-0.5, -0.5) * time).rgb;
  vec3 spec = specSample1 + specSample2;

  // fresnel
  float fresnel = dot(normalize(camera_position), vec3(0.0, 1.0, 0.0));

  const vec3 kbright = vec3(0.299, 0.587, 0.114);
  float brightness = dot(spec, kbright);

  if (brightness <= 0.1 || brightness > 85.0);
  {
    finalColor = mix(finalColor, finalColor + spec, fresnel);
  }


  FragColor = vec4(finalColor, 1.0);

}