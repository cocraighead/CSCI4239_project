//  CSCIx239 library
//  Willem A. (Vlakkies) Schreuder
#include "CSCIx239.h"

//
//  Enable light 0 and draw sphere to represent light position
//
void Lighting(float x,float y,float z,float ambient,float diffuse,float specular)
{
   //  Translate intensity to color vectors
   float Ambient[]   = {ambient ,ambient ,ambient ,1.0};
   float Diffuse[]   = {diffuse ,diffuse ,diffuse ,1.0};
   float Specular[]  = {specular,specular,specular,1.0};
   //  Light direction
   float Position[]  = {x,y,z,1};

   //  Draw light position as ball (still no lighting here)
   glColor3f(1,1,1);
   Sphere(x,y,z,0.1,0,8,0);

   //  OpenGL should normalize normal vectors
   glEnable(GL_NORMALIZE);
   //  Enable lighting
   glEnable(GL_LIGHTING);
   //  glColor sets ambient and diffuse color materials
   glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);

   //  Enable light 0
   glEnable(GL_LIGHT0);
   //  Set ambient, diffuse, specular components and position of light 0
   glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
   glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
   glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
   glLightfv(GL_LIGHT0,GL_POSITION,Position);
}
