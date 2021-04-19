//  neonsign geometry shader


#version 150 compatibility
#extension GL_EXT_geometry_shader4 : enable
layout(lines) in;
layout(triangle_strip,max_vertices=4) out;

uniform float r;

out vec4 Color;

void main()
{
   //  Copy Color
   Color = gl_FrontColorIn[0];

   // 2 points of the line
   vec4 p0 = gl_PositionIn[0];
   vec4 p1 = gl_PositionIn[1];
   // find rotation
   // vec3 a = vec3(1,0,0);
   vec3 b = vec3(0.0,0.0,0.0);
   if(p1.x - p0.x > 0){
      b = vec3(-1*(p1.y - p0.y),(p1.x - p0.x),0);
   }else{
      b = vec3((p1.y - p0.y),-1*(p1.x - p0.x),0);
   }
   b = r*normalize(b);

   // four vecs to make a rectangle from triangle_strip
   // 1
   vec4 v = vec4(p1.x-b.x, p1.y-b.y, 0, 1.0);
   gl_Position = gl_ModelViewProjectionMatrix*v;
   EmitVertex();
   // 2
   v = vec4(p0.x-b.x, p0.y-b.y, 0, 1.0);
   gl_Position = gl_ModelViewProjectionMatrix*v;
   EmitVertex();
   // 3
   v = vec4(p1.x+b.x, p1.y+b.y, 0, 1.0);
   gl_Position = gl_ModelViewProjectionMatrix*v;
   EmitVertex();
   // 4
   v = vec4(p0.x+b.x, p0.y+b.y, 0, 1.0);
   gl_Position = gl_ModelViewProjectionMatrix*v;
   EmitVertex();
   EndPrimitive();
}
