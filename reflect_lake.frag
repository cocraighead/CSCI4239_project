// lake reflection
#version 120

uniform float dX;
uniform float dY;
uniform sampler2D tex_frame0;
uniform sampler2D tex_frame1;

float is_greenscreen(vec4 c){
   if(c.r < 0.01 && c.g > 0.99 && c.b < 0.01)
      return 1.0;
   return 0.0;
}

void main()
{
   vec4 ret_color = texture2D(tex_frame0,gl_TexCoord[0].st);
   if(is_greenscreen(ret_color) == 1.0){
      ret_color = texture2D(tex_frame1,vec2(1-gl_TexCoord[0].s,gl_TexCoord[0].t));
   }
   gl_FragColor = ret_color;
}
