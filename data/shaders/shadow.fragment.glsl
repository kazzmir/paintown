#ifdef GL_ES
precision mediump float;
#endif
uniform sampler2D al_tex;
uniform bool al_use_tex;
uniform vec4 shadow; 
varying vec4 varying_color;
varying vec2 varying_texcoord;

void main(){
  if (al_use_tex){
    vec4 sample = texture2D(al_tex, varying_texcoord);
    gl_FragColor = vec4(shadow.r, shadow.g, shadow.b, shadow.a * sample.a);
  } else {
    gl_FragColor = varying_color;
  }
}
