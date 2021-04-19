//  neonsign geometry shader


#version 150 compatibility
#extension GL_EXT_geometry_shader4 : enable
layout(lines) in;
layout(triangle_strip,max_vertices=24) out;

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
   vec3 a = vec3(1,0,0);
   vec3 b = vec3(0.0,0.0,0.0);
   if(p1.x - p0.x > 0){
      b = vec3(-1*(p1.y - p0.y),(p1.x - p0.x),0);
   }else{
      b = vec3((p1.y - p0.y),-1*(p1.x - p0.x),0);
   }
   b = normalize(b);
   float pheta = acos(dot(a,b));
   // rotation around z mat3
   mat3 rotz = mat3(cos(pheta), sin(pheta), 0.0,  // 1. column
                   -sin(pheta), cos(pheta), 0.0,  // 2. column
                   0.0,         0.0,        1.0); // 3. column
   
   // for loop to loop around the line
   float pi = 3.14159;
   for(int angle=0;angle<360;angle+=60)
   {
      // convert to rads
      float angle_rad = (pi/180)*angle;
      float angle_rad2 = (pi/180)*(angle+60);
      // rotated unit circle
      vec3 unit1 = vec3(r*cos(angle_rad),0,r*sin(angle_rad));
      unit1 = rotz*unit1;
      vec3 unit2 = vec3(r*cos(angle_rad2),0,r*sin(angle_rad2));
      unit2 = rotz*unit2;

      // four vecs to make a rectangle triangle triangle_strip
      // 1
      vec4 v = vec4(unit1 + p0.xyz, 1.0);
      gl_Position = gl_ModelViewProjectionMatrix*v;
      EmitVertex();
      // 2
      v = vec4(unit1 + p1.xyz, 1.0);
      gl_Position = gl_ModelViewProjectionMatrix*v;
      EmitVertex();
      // 3
      v = vec4(unit2 + p0.xyz, 1.0);
      gl_Position = gl_ModelViewProjectionMatrix*v;
      EmitVertex();
      // 4
      v = vec4(unit2 + p1.xyz, 1.0);
      gl_Position = gl_ModelViewProjectionMatrix*v;
      EmitVertex();
   }
   EndPrimitive();
}
