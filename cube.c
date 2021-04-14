//  CSCIx239 library
//  Willem A. (Vlakkies) Schreuder
#include "CSCIx239.h"

//
//  Solid unit cube
//
void SolidCube()
{
   //  Set colors
   float color[][4] = {{0,0,0,0},{1,0,0,0},{1,1,0,0},{0,1,0,0},{0,1,1,0},{0,0,1,0},{1,0,1,0},{1,1,1,1},{.5,.5,.5,0}};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,32.0);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,color[7]);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,color[0]);

   //  Front
   glColor4fv(color[8]);
   glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,color[1]);
   glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,color[1]);
   glNormal3f( 0, 0, 1);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
   glTexCoord2f(1,0); glVertex3f(+1,-1, 1);
   glTexCoord2f(1,1); glVertex3f(+1,+1, 1);
   glTexCoord2f(0,1); glVertex3f(-1,+1, 1);
   glEnd();

   //  Back
   glColor4fv(color[8]);
   glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,color[2]);
   glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,color[2]);
   glNormal3f( 0, 0,-1);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,-1);
   glEnd();

   //  Right
   glColor4fv(color[8]);
   glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,color[3]);
   glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,color[3]);
   glNormal3f(+1, 0, 0);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,+1);
   glEnd();

   //  Left
   glColor4fv(color[8]);
   glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,color[4]);
   glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,color[4]);
   glNormal3f(-1, 0, 0);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();

   //  Top
   glColor4fv(color[8]);
   glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,color[5]);
   glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,color[5]);
   glNormal3f( 0,+1, 0);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,+1,+1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();

   //  Bottom
   glColor4fv(color[8]);
   glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,color[6]);
   glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,color[6]);
   glNormal3f( 0,-1, 0);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,-1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,-1,+1);
   glEnd();
}

//
//  Textured unit cube
//
void TexturedCube(int tex)
{
   //  Draw with texture
   if (tex)
   {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D,tex);
      glEnable(GL_TEXTURE_2D);
      SolidCube();
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, 0);
      glDisable(GL_TEXTURE_2D);
   }
   //  Draw without texture
   else
      SolidCube();
}

//
//  General cube
//
void Cube(float x,float y,float z , float dx,float dy,float dz, float th,float ph , int tex)
{

   //  Transform
   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(ph,1,0,0);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Draw textured cube
   TexturedCube(tex);
   // Restore
   glPopMatrix();
}
