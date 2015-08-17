#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D al_tex;
uniform bool al_use_tex;
varying vec4 varying_color; 
varying vec2 varying_texcoord; 

uniform int time;
uniform vec4 add;
uniform vec4 multiply;
uniform vec4 sin_;
uniform int period;
uniform bool invert;
uniform float color;

void main(){ 
  if (al_use_tex){
    vec4 tex_color = varying_color * texture2D(al_tex, varying_texcoord); 
    float PI = 3.14159265358979323846264;

    if (color < 1.0){
        tex_color.r = (tex_color.r * ((1.0 - 0.299) * color + 0.299)) + 16.0 / 255.0;
        tex_color.g = (tex_color.g * ((1.0 - 0.587) * color + 0.587)) + 16.0 / 255.0;
        tex_color.b = (tex_color.b * ((1.0 - 0.114) * color + 0.114)) + 16.0 / 255.0;
    }

    if (invert){
        tex_color.r = 1.0 - tex_color.r;
        tex_color.g = 1.0 - tex_color.g;
        tex_color.b = 1.0 - tex_color.b;
    }

    if (period > 0){
        float angle = sin(2.0 * PI * float(time) / float(period));
        tex_color.r = (tex_color.r + add.r + sin_.r * angle) * multiply.r;
        tex_color.g = (tex_color.g + add.g + sin_.g * angle) * multiply.g;
        tex_color.b = (tex_color.b + add.b + sin_.b * angle) * multiply.b;

        // tex_color.rgb = (tex_color.rgb + add.rgb + sin_.rgb * angle) * multiply.rgb;
    } else {
        tex_color.r = (tex_color.r + add.r) * multiply.r;
        tex_color.g = (tex_color.g + add.g) * multiply.g;
        tex_color.b = (tex_color.b + add.b) * multiply.b;
    }

    gl_FragColor = tex_color;
  } else {
    gl_FragColor = varying_color;
  }
} 
