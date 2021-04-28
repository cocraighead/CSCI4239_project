// lake reflection frag shader
#version 120

uniform sampler2D tex_frame0;
uniform sampler2D tex_frame1;
uniform sampler2D tex_frame2;

// is pixel green
float is_greenscreen(vec4 c){
   if(c.r < 0.01 && c.g > 0.99 && c.b < 0.01)
      return 1.0;
   return 0.0;
}

// is pixel black
float is_black(vec4 c){
   if(c.r < 0.001 && c.g < 0.001 && c.b < 0.001)
      return 1.0;
   return 0.0;
}

void main()
{
   // replace areas with area from other frames
   vec4 ret_color = texture2D(tex_frame0,gl_TexCoord[0].st);
   if(is_greenscreen(ret_color) == 1.0){
      ret_color = texture2D(tex_frame1,vec2(1-gl_TexCoord[0].s,gl_TexCoord[0].t));
   }
   if(is_black(texture2D(tex_frame2,vec2(gl_TexCoord[0].st))) == 0.0){
      ret_color += texture2D(tex_frame2,vec2(gl_TexCoord[0].st));
   }
   gl_FragColor = ret_color;
}
