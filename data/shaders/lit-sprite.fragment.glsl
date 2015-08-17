#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D al_tex;
uniform float light_intensity; 
uniform vec4 light_color; 
uniform bool al_use_tex;
varying vec4 varying_color; 
varying vec2 varying_texcoord; 
void main(){ 
  if (al_use_tex){
    vec4 tex_color = varying_color * texture2D(al_tex, varying_texcoord); 
    vec4 blended_light_color = tex_color.a * light_color; 
    gl_FragColor = vec4(tex_color.rgb + (blended_light_color.rgb - tex_color.rgb) * light_intensity, tex_color.a); 
  } else {
    gl_FragColor = varying_color;
  }
} 
