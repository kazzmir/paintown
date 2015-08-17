#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D al_tex;
uniform sampler2D remap;
uniform bool al_use_tex;
varying vec4 varying_color; 
varying vec2 varying_texcoord; 
void main(){ 
  if (al_use_tex){
    vec4 tex_color = texture2D(al_tex, varying_texcoord); 
    vec4 color = tex_color;
    int i;
    vec2 dimensions = textureSize(remap, 0).xy;

    for (i = 0; i < dimensions.y; i++){
      vec2 position = vec2(0 * 1 / dimensions.x, i * 1.0 / dimensions.y);
      vec4 test = texture2D(remap, position);
      vec4 new_color = texture2D(remap, vec2(1.0 * 1.0 / dimensions.x, i * 1.0 / dimensions.y));
      float epsilon = 0.05;
      if (distance(test.r, tex_color.r) < epsilon &&
          distance(test.g, tex_color.g) < epsilon &&
          distance(test.b, tex_color.b) < epsilon &&
          distance(test.a, tex_color.a) < epsilon
          ){
        color = new_color;
        break;
      }
    }

    gl_FragColor = varying_color * color;
  } else {
    gl_FragColor = varying_color;
  }
} 
