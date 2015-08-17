#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D al_tex;
uniform bool al_use_tex;
varying vec4 varying_color; 
varying vec2 varying_texcoord; 

uniform vec4 bright;
uniform vec4 contrast;
uniform vec4 post;
uniform vec4 extraAdd;
uniform vec4 extraMultiplier;
uniform float extra;

void main(){ 
  if (al_use_tex){
    vec4 tex_color = varying_color * texture2D(al_tex, varying_texcoord); 

    // tex_color.r = (tex_color.r * contrast.r) + bright.r * contrast.r + post.r;
    tex_color.r = (tex_color.r + bright.r) * contrast.r + post.r;
    tex_color.r += extraAdd.r * extra;
    tex_color.r *= pow(extraMultiplier.r, extra);
    
    // tex_color.g = (tex_color.g * contrast.g) + bright.g * contrast.g + post.g;
    tex_color.g = (tex_color.g + bright.g) * contrast.g + post.g;
    tex_color.g += extraAdd.g * extra;
    tex_color.g *= pow(extraMultiplier.g, extra);
    
    // tex_color.b = (tex_color.b * contrast.b) + bright.b * contrast.b + post.b;
    tex_color.b = (tex_color.b + bright.b) * contrast.b + post.b;
    tex_color.b += extraAdd.b * extra;
    tex_color.b *= pow(extraMultiplier.b, extra);

    if (tex_color.a == 0.0){
        tex_color.r = 0.0;
        tex_color.g = 0.0;
        tex_color.b = 0.0;
    }

    gl_FragColor = tex_color;
  } else {
    gl_FragColor = varying_color;
  }
} 
