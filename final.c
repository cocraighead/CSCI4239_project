/*
 * Final Project
 *
 *
 *  Key bindings:
 *  arrows     Change view angle
 *  space bar   Change view height
 *  m          Toggle scene
 *  0          Set th and ph to 0 - overhead view angle
 *  l          Turn light movement on/off
 *  [ and ]    Change light position
 *  { and }    Change light height
 *  wasdrf     Move an object wtih gex/gey/gez  
 *  ESC        Exit
 */
#include "CSCIx239.h"
// View globals
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int fov=57;       //  Camera Fov 
float asp=1;      //  Screen aspect ratio
float dim = 32;      //  World dimension
// Scene modes globals
#define MODE 3
int mode = 0;
const char* text[] = {"Bar","Camp Ground","Any"};
// Shader Globals
int shader[] = {0,0,0,0};  //  Shaders
// light globals
int move_light = 1;      // if the light will be moving
int zh=0;                //  Light angle
int light_radius = 3;    // light radius 
float light_elv=1.1;     //  Light elevation
float l_ambient =0.3;    // Light properties
float l_diffuse = 0.5;
float l_specular = 0.8;
// Texture gloabls
int tex[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};     //  Textures
// Firefly globals
int N=128;       //  Number of bodies
int src=0;        //  Offset of first fly in source
int dst=0;        //  Offset of first fly in destination
double vel=0.1;   //  Relative speed
int box_size = 8; // fire flys stuck in a box

// global edit
int gex =0;
int gey =0;
int gez =0;

// move camera
float PX = 0;  // Player x location
float PZ = 0;  // Player z location
float PY = 0;
float PLX = 0; // Players looking at x location
float PLZ = -1; // Players looking at z location
float PLY = 0;
int pheta = 0; // Angle the player is turned at

void normmap_quad(float x, float y, float z, float dx, float dy, float dz, float rx, float ry, float rz, int image_tex, int norm_tex)
{
   //  Select shader
   glUseProgram(shader[1]);
   //  Walls material and color
   float color[][4] = {{0,0,0,0},{.5,.5,.5,0},{1,1,0,0},{0,1,0,0},{0,1,1,0},{0,0,1,0},{1,0,1,0},{1,1,1,1}};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,32.0);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,color[7]);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,color[0]);
   glColor4fv(color[1]);
   glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,color[1]);
   glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,color[1]);
   // set texture uniforms
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, image_tex);
   int id = glGetUniformLocation(shader[1],"tex");
   glUniform1i(id,0);
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, norm_tex);
   id = glGetUniformLocation(shader[1],"norms");
   glUniform1i(id,1);
   // cube front face
   // save transformation
   glPushMatrix();
   // transformations
   glTranslated(x,y,z);
   glRotated(rx,1,0,0);
   glRotated(ry,0,1,0);
   glRotated(rz,0,0,1);
   glScaled(dx,dy,1);
   // get current model view matrix
   float modelview_matrix[16]; 
   glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix); 
   // get Normal matrix
   float normal_matrix[9];
   mat4normalMatrix(modelview_matrix, normal_matrix);
   id = glGetUniformLocation(shader[1],"NormalMatrix");
   glUniformMatrix3fv(id,1,0,normal_matrix);
   // Wall
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 0);
   glTexCoord2f(1,0); glVertex3f(+1,-1, 0);
   glTexCoord2f(1,1); glVertex3f(+1,+1, 0);
   glTexCoord2f(0,1); glVertex3f(-1,+1, 0);
   glEnd();
   glPopMatrix();
   // unbind textures
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, 0);
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, 0);
   //  Revert to fixed pipeline
   glUseProgram(0);
}

void normmap_cube(float x, float y, float z, float dx, float dy, float dz, float rx, float ry, float rz, int image_tex, int norm_tex)
{
   //  Select shader
   glUseProgram(shader[1]);
   //  Walls material and color
   float color[][4] = {{0,0,0,0},{.5,.5,.5,0},{1,1,0,0},{0,1,0,0},{0,1,1,0},{0,0,1,0},{1,0,1,0},{1,1,1,1}};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,32.0);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,color[7]);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,color[0]);
   glColor4fv(color[1]);
   glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,color[1]);
   glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,color[1]);
   // set texture uniforms
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, image_tex);
   int id = glGetUniformLocation(shader[1],"tex");
   glUniform1i(id,0);
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, norm_tex);
   id = glGetUniformLocation(shader[1],"norms");
   glUniform1i(id,1);
   // transformation mat
   float special_mat[16];
   mat4identity(special_mat);
   mat4scale(special_mat,dx,dy,dz);
   mat4rotate(special_mat,-rz,0,0,1);
   mat4rotate(special_mat,-ry,0,1,0);
   mat4rotate(special_mat,-rx,1,0,0);
   // cube front face
   // save transformation
   glPushMatrix();
   float special_vec_front[16] = {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0};
   mat4multMatrix(special_vec_front,special_mat);
   // transformations
   glTranslated(x+special_vec_front[0],y+special_vec_front[4],z+special_vec_front[8]);
   glRotated(rx,1,0,0);
   glRotated(ry,0,1,0);
   glRotated(rz,0,0,1);
   glScaled(dx,dy,1);
   // get current model view matrix
   float modelview_matrix[16]; 
   glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix); 
   // get Normal matrix
   float normal_matrix[9];
   mat4normalMatrix(modelview_matrix, normal_matrix);
   id = glGetUniformLocation(shader[1],"NormalMatrix");
   glUniformMatrix3fv(id,1,0,normal_matrix);
   // Wall
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 0);
   glTexCoord2f(1,0); glVertex3f(+1,-1, 0);
   glTexCoord2f(1,1); glVertex3f(+1,+1, 0);
   glTexCoord2f(0,1); glVertex3f(-1,+1, 0);
   glEnd();
   glPopMatrix();
   // cube face back
   // save transformation
   glPushMatrix();
   float special_vec_back[16] = {0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0};
   mat4multMatrix(special_vec_back,special_mat);
   // transformations
   glTranslated(x+special_vec_back[0],y+special_vec_back[4],z+special_vec_back[8]);
   glRotated(rx,1,0,0);
   glRotated(ry,0,1,0);
   glRotated(rz,0,0,1);
   glRotated(+180,0,1,0);
   glScaled(dx,dy,1);
   // get current model view matrix
   glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix); 
   // get Normal matrix
   mat4normalMatrix(modelview_matrix, normal_matrix);
   id = glGetUniformLocation(shader[1],"NormalMatrix");
   glUniformMatrix3fv(id,1,0,normal_matrix);
   // Wall
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 0);
   glTexCoord2f(1,0); glVertex3f(+1,-1, 0);
   glTexCoord2f(1,1); glVertex3f(+1,+1, 0);
   glTexCoord2f(0,1); glVertex3f(-1,+1, 0);
   glEnd();
   glPopMatrix();
   // cube face left
   // save transformation
   glPushMatrix();
   float special_vec_left[16] = {-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
   mat4multMatrix(special_vec_left,special_mat);
   // transformations
   glTranslated(x+special_vec_left[0],y+special_vec_left[4],z+special_vec_left[8]);
   glRotated(rx,1,0,0);
   glRotated(ry,0,1,0);
   glRotated(rz,0,0,1);
   glRotated(-90,0,1,0);
   glScaled(dz,dy,1);
   // get current model view matrix
   glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix); 
   // get Normal matrix
   mat4normalMatrix(modelview_matrix, normal_matrix);
   id = glGetUniformLocation(shader[1],"NormalMatrix");
   glUniformMatrix3fv(id,1,0,normal_matrix);
   // Wall
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 0);
   glTexCoord2f(1,0); glVertex3f(+1,-1, 0);
   glTexCoord2f(1,1); glVertex3f(+1,+1, 0);
   glTexCoord2f(0,1); glVertex3f(-1,+1, 0);
   glEnd();
   glPopMatrix();
   // cube face right
   // save transformation
   glPushMatrix();
   float special_vec_right[16] = {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
   mat4multMatrix(special_vec_right,special_mat);
   // transformations
   glTranslated(x+special_vec_right[0],y+special_vec_right[4],z+special_vec_right[8]);
   glRotated(rx,1,0,0);
   glRotated(ry,0,1,0);
   glRotated(rz,0,0,1);
   glRotated(+90,0,1,0);
   glScaled(dz,dy,1);
   // get current model view matrix
   glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix); 
   // get Normal matrix
   mat4normalMatrix(modelview_matrix, normal_matrix);
   id = glGetUniformLocation(shader[1],"NormalMatrix");
   glUniformMatrix3fv(id,1,0,normal_matrix);
   // Wall
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 0);
   glTexCoord2f(1,0); glVertex3f(+1,-1, 0);
   glTexCoord2f(1,1); glVertex3f(+1,+1, 0);
   glTexCoord2f(0,1); glVertex3f(-1,+1, 0);
   glEnd();
   glPopMatrix();
   // cube face top
   // save transformation
   glPushMatrix();
   float special_vec_top[16] = {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0};
   mat4multMatrix(special_vec_top,special_mat);
   // transformations
   glTranslated(x+special_vec_top[0],y+special_vec_top[4],z+special_vec_top[8]);
   glRotated(rx,1,0,0);
   glRotated(ry,0,1,0);
   glRotated(rz,0,0,1);
   glRotated(-90,1,0,0);
   glScaled(dx,dz,1);
   // get current model view matrix
   glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix); 
   // get Normal matrix
   mat4normalMatrix(modelview_matrix, normal_matrix);
   id = glGetUniformLocation(shader[1],"NormalMatrix");
   glUniformMatrix3fv(id,1,0,normal_matrix);
   // Wall
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 0);
   glTexCoord2f(1,0); glVertex3f(+1,-1, 0);
   glTexCoord2f(1,1); glVertex3f(+1,+1, 0);
   glTexCoord2f(0,1); glVertex3f(-1,+1, 0);
   glEnd();
   glPopMatrix();
   // cube face bot
   // save transformation
   glPushMatrix();
   float special_vec_bot[16] = {0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0};
   mat4multMatrix(special_vec_bot,special_mat);
   // transformations
   glTranslated(x+special_vec_bot[0],y+special_vec_bot[4],z+special_vec_bot[8]);
   glRotated(rx,1,0,0);
   glRotated(ry,0,1,0);
   glRotated(rz,0,0,1);
   glRotated(+90,1,0,0);
   glScaled(dx,dz,1);
   // get current model view matrix
   glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix); 
   // get Normal matrix
   mat4normalMatrix(modelview_matrix, normal_matrix);
   id = glGetUniformLocation(shader[1],"NormalMatrix");
   glUniformMatrix3fv(id,1,0,normal_matrix);
   // Wall
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 0);
   glTexCoord2f(1,0); glVertex3f(+1,-1, 0);
   glTexCoord2f(1,1); glVertex3f(+1,+1, 0);
   glTexCoord2f(0,1); glVertex3f(-1,+1, 0);
   glEnd();
   glPopMatrix();
   // unbind textures
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, 0);
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, 0);
   //  Revert to fixed pipeline
   glUseProgram(0);
}

void record_disc(float x, float y, float z, float dx, float dy, float dz, float rx, float ry, float rz, int image_tex, int norm_tex)
{
   //  Select shader
   glUseProgram(shader[1]);
   //  Walls material and color
   float color[][4] = {{0,0,0,0},{.5,.5,.5,0},{1,1,0,0},{0,1,0,0},{0,1,1,0},{0,0,1,0},{1,0,1,0},{1,1,1,1},{.7,.7,.7,0},{.8,.8,.8,0}};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,96.0);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,color[8]);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,color[0]);
   glColor4fv(color[1]);
   glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,color[1]);
   glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,color[9]);
   // set texture uniforms
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, image_tex);
   int id = glGetUniformLocation(shader[1],"tex");
   glUniform1i(id,0);
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, norm_tex);
   id = glGetUniformLocation(shader[1],"norms");
   glUniform1i(id,1);
   // save transformation
   glPushMatrix();
   // transformations
   glTranslated(x,y,z);
   glRotated(rx,1,0,0);
   glRotated(ry,0,1,0);
   glRotated(rz,0,0,1);
   glScaled(dx,dy,1);
   // get current model view matrix
   float modelview_matrix[16]; 
   glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix); 
   // get Normal matrix
   float normal_matrix[9];
   mat4normalMatrix(modelview_matrix, normal_matrix);
   id = glGetUniformLocation(shader[1],"NormalMatrix");
   glUniformMatrix3fv(id,1,0,normal_matrix);
   // disc made from triangles
   glBegin(GL_TRIANGLES);
   for(int angle=0;angle<360;angle+=15){
      glTexCoord2f(.5,.5); glVertex3f(0,0,0);
      float a_cos = Cos(angle);
      float a_sin = Sin(angle);
      glTexCoord2f(a_cos/2+0.5,a_sin/2+0.5); glVertex3f(a_cos,a_sin, 0);
      a_cos = Cos(angle+15);
      a_sin = Sin(angle+15);
      glTexCoord2f(a_cos/2+0.5,a_sin/2+0.5); glVertex3f(a_cos,a_sin, 0);
   }
   glEnd();
   glPopMatrix();
   // unbind textures
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, 0);
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, 0);
   //  Revert to fixed pipeline
   glUseProgram(0);
}

// record player
void Recordplayer(int x, int y, int z)
{
   record_disc(x+0,y+0.08,z+0, 0.85,0.85,1, -90,0,zh, tex[7],tex[8]);
   //  Draw cube 1 - bottem
   normmap_cube(x+0,y+-.525,z+0, 1.55,.025,1.05, 0,0,0, tex[5],tex[6]);
   // // draw cube 2 - front
   normmap_cube(x+0,y+0,z+1, 1.5,0.5,0.025, 0,0,0, tex[5],tex[6]);
   // // draw cube 3 - right
   normmap_cube(x+1.5,y+0,z+0, 0.025,0.5,1, 0,0,0, tex[5],tex[6]);
   // // draw cube 4 - back 
   normmap_cube(x+0,y+0,z+-1, 1.5,0.5,0.025, 0,0,0, tex[5],tex[6]);
   // // draw cube 5 - left
   normmap_cube(x+-1.5,y+0,z+0, 0.025,0.5,1, 0,0,0, tex[5],tex[6]);
   // // draw cube 6 - lid
   normmap_cube(x+0,y+1.37,z+-.5, 1.5,0.025,1, -60,0,0, tex[5],tex[6]);
   // // draw cube 7 - box fill
   normmap_cube(x+0,y+-.25,z+0, 1.5,.3,1, 0,0,0, tex[5],tex[6]);
}

void table(int x, int y, int z)
{
   // table top
   normmap_cube(x+0,y+4.5,z+0, 1.2,.15,1, 0,0,0, tex[5],tex[6]);
   // one table leg
   Cube(x+0,y+2.24,z+0, .1,2.24,.1, 0,0, tex[5]);
   // center base
   Cube(x+0,y+.15,z+0, .4,.3,.4, 0,0, tex[5]);
   // z bottom cross
   Cube(x+0,y+.1,z+0, .1,.2,1.2, 0,0, tex[5]);
   // x bottem cross
   Cube(x+0,y+.1,z+0, 1.2,.2,.1, 0,0, tex[5]);
}

void chair(int x, int y, int z, int ry)
{
   glPushMatrix();
   glRotated(ry,0,1,0);
   //legs - fl, fr , bl , br
   Cube(x+-.81,y+1.8,z+.71, .09,1.8,.09, 0,0, tex[5]);
   Cube(x+.81,y+1.8,z+.71, .09,1.8,.09, 0,0, tex[5]);
   Cube(x+-.81,y+1.8,z+-.71, .09,1.8,.09, 0,0, tex[5]);
   Cube(x+.81,y+1.8,z+-.71, .09,1.8,.09, 0,0, tex[5]);
   // seat
   Cube(x+0,y+3.63,z+0, .9,.08,.8, 0,0, tex[5]);
   // conector
   Cube(x+0,y+3.86,z+.9, .8,.2,.04, 0,30, tex[5]);
   // back
   Cube(x+0,y+4.5,z+1.0, 1,.5,.06, 0,0, tex[5]);
   glPopMatrix();
}

void Bar()
{
   // floor 
   for(int i=0;i<8;i++){
      for(int j=0;j<12;j++){
         normmap_quad(4*j-22,0,4*i-14,2,2,1,-90,0,0,tex[0],tex[1]);
      }
   }
   // behind bar wall
   for(int i=0;i<3;i++){
      for(int j=0;j<12;j++){
         normmap_quad(4*j-22,4*i+1,-16,2,2,1,0,0,0,tex[0],tex[1]);
      }
   }
   // left wall
   for(int i=0;i<3;i++){
      for(int j=0;j<8;j++){
         normmap_quad(-24,4*i+1,4*j-14,2,2,1,0,90,0,tex[0],tex[1]);
      }
   }
   //right wall
   for(int i=0;i<3;i++){
      for(int j=0;j<8;j++){
         normmap_quad(24,4*i+1,4*j-14,2,2,1,0,-90,0,tex[0],tex[1]);
      }
   }
   // tables wall
   for(int i=0;i<3;i++){
      for(int j=0;j<12;j++){
         normmap_quad(4*j-22,4*i+1,16,2,2,1,0,180,0,tex[0],tex[1]);
      }
   }
   // Roof
   Cube(0,17.8,-8.5, 24,10,1, 0,53.13, tex[12]);
   Cube(0,17.8,8.5, 24,10,1, 0,-53.13, tex[12]);
   // roof side walls
   // left wall
   for(int i=0;i<3;i++){
      for(int j=0;j<8;j++){
         Cube(-25,4*i+13,4*j-14, 1,2,2, 0,0, tex[0]);
      }
   }
   //right wall
   for(int i=0;i<3;i++){
      for(int j=0;j<8;j++){
         Cube(25,4*i+13,4*j-14, 1,2,2, 0,0, tex[0]);
      }
   }
   // beams - horizontal
   for(int i=0;i<4;i++){
      Cube(-20+i*10,12.5,0, .8,.5,16, 0,0, tex[5]);
   }
   // beams - verticle
   for(int i=0;i<4;i++){
      Cube(-20+i*10,18.1,0, .5,6,.5, 0,0, tex[5]);
   }
   // bar counter
   normmap_cube(0+-21,4,-9, 4, .25, 2, 0, 0, 0, tex[5], tex[6]);
   normmap_cube(8+-21,4,-9, 4, .25, 2, 0, 0, 0, tex[5], tex[6]);
   normmap_cube(16+-21,4,-9, 4, .25, 2, 0, 0, 0, tex[5], tex[6]);
   normmap_cube(24+-21,4,-9, 4, .25, 2, 0, 0, 0, tex[5], tex[6]);
   // bar stone block
   normmap_cube(0+-21.5,2,-9, 4, 2, 1.7, 0, 0, 0, tex[0], tex[1]);
   normmap_cube(8+-21.5,2,-9, 4, 2, 1.7, 0, 0, 0, tex[0], tex[1]);
   normmap_cube(16+-21.5,2,-9, 4, 2, 1.7, 0, 0, 0, tex[0], tex[1]);
   normmap_cube(24+-21.5,2,-9, 4, 2, 1.7, 0, 0, 0, tex[0], tex[1]);
   // Tables and chairs
   table(-16,0,11); chair(-11,0,-13,90); chair(11,0,18,-90);
   table(-2,0,11); chair(-11,0,0,90); chair(11,0,4,-90);
   table(12,0,11); chair(-11,0,14.5,90); chair(11,0,-9,-90);
   table(18,0,-7); chair(18,0,-5,0); chair(-18,0,9,180);
   // bar chairs
   chair(-11,0,-5,0);
   chair(-3,0,-5,0);
   // record player 
   table(21,0,4);
   Recordplayer(-18,5,-8);
}

//  Fly
typedef struct
{
   float x,y,z;  //  Position
   float u,v,w;  //  Velocity
   float r,g,b;  //  Color
}  Fly;
// Fly array
Fly* flys=NULL;

//
//  Advance time one time step for fly k
//
void Move(int k)
{
   int k0 = k+src;
   int k1 = k+dst;
   float dt = 1e-3;
   // random acceleration
   float ax = (rand()%1000)/10000.0 - .05;
   float ay = (rand()%1000)/10000.0 - .05;
   float az = (rand()%1000)/10000.0 - .05;
   //  Update velocity
   // flip vel if fly is close to box edge
   if(flys[k0].x > box_size || flys[k0].x < -box_size){
      flys[k0].u *= -1;
   }
   if(flys[k0].y > box_size || flys[k0].y < -box_size){
      flys[k0].v *= -1;
   }
   if(flys[k0].z > box_size || flys[k0].z < -box_size){
      flys[k0].w *= -1;
   }
   flys[k1].u = flys[k0].u + ax;
   flys[k1].v = flys[k0].v + ay;
   flys[k1].w = flys[k0].w + az;
   //  Update position
   flys[k1].x = flys[k0].x + dt*flys[k1].u;
   flys[k1].y = flys[k0].y + dt*flys[k1].v;
   flys[k1].z = flys[k0].z + dt*flys[k1].w;
}

//
//  Advance time one time step
//
void Step()
{
   int k;
   //  Switch source and destination
   src = src?0:N;
   dst = dst?0:N;
   for (k=0;k<N;k++)
      Move(k);
}

//
//  Scaled random value
//
void rand3(float Sx,float Sy,float Sz,float* X,float* Y,float* Z)
{
   float x = 0;
   float y = 0;
   float z = 0;
   float d = 2;
   while (d>1)
   {
      x = rand()/(0.5*RAND_MAX)-1;
      y = rand()/(0.5*RAND_MAX)-1;
      z = rand()/(0.5*RAND_MAX)-1;
      d = x*x+y*y+z*z;
   }
   *X = Sx*x;
   *Y = Sy*y;
   *Z = Sz*z;
}

//
//  Initialize fireflys
//
void InitLoc()
{
   int k;
   //  Allocate room for twice as many bodies to facilitate ping-pong
   if (!flys) flys = malloc(2*N*sizeof(Fly));
   if (!flys) Fatal("Error allocating memory for %d flys\n",N);
   src = N;
   dst = 0;
   //  Assign random locations
   for (k=0;k<N;k++)
   {
      rand3(dim/2,dim/2,dim/3,&flys[k].x,&flys[k].y,&flys[k].z);
      rand3(vel,vel,vel,&flys[k].u,&flys[k].v,&flys[k].w);
      switch (k%2)
      {
         case 0:
           flys[k].r = 0;
           flys[k].g = 0;
           flys[k].b = 0;
           break;
         case 1:
           flys[k].r = 1.0;
           flys[k].g = 0.9;
           flys[k].b = 0.5;
           break;
      }
      flys[k+N].r = flys[k].r;
      flys[k+N].g = flys[k].g;
      flys[k+N].b = flys[k].b;
   }
}

void terrain()
{
   //  Select shader
   glUseProgram(shader[3]);
   //  Walls material and color
   float color[][4] = {{0,0,0,0},{.5,.5,.5,0},{1,1,0,0},{0,1,0,0},{0,1,1,0},{0,0,1,0},{1,0,1,0},{.3,.3,.3,0}};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,32.0);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,color[7]);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,color[0]);
   glColor4fv(color[1]);
   glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,color[1]);
   glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,color[1]);
   // set texture uniforms
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, tex[4]);
   int id = glGetUniformLocation(shader[3],"tex");
   glUniform1i(id,0);
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, tex[10]);
   id = glGetUniformLocation(shader[3],"norms");
   glUniform1i(id,1);
   glActiveTexture(GL_TEXTURE2);
   glBindTexture(GL_TEXTURE_2D, tex[9]);
   id = glGetUniformLocation(shader[3],"heights");
   glUniform1i(id,2);
   glActiveTexture(GL_TEXTURE3);
   glBindTexture(GL_TEXTURE_2D, tex[11]);
   id = glGetUniformLocation(shader[3],"alt_tex");
   glUniform1i(id,3);
   // save transformation
   glPushMatrix();
   // transformations
   glTranslated(0,-10,0);
   glRotated(-90,1,0,0);
   glRotated(0,0,1,0);
   glRotated(0,0,0,1);
   glScaled(12,24,10);
   // get current model view matrix
   float modelview_matrix[16]; 
   glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix); 
   // get Normal matrix
   float normal_matrix[9];
   mat4normalMatrix(modelview_matrix, normal_matrix);
   id = glGetUniformLocation(shader[3],"NormalMatrix");
   glUniformMatrix3fv(id,1,0,normal_matrix);
   // Grid
   glBegin(GL_TRIANGLES);
   int grid_size = 16;
   int blx = -grid_size/2;
   int bly = -grid_size/2;
   float di = 1.0/grid_size;
   float dj = 1.0/grid_size;
   for(int i=0;i<grid_size;i++){
      for(int j=0;j<grid_size;j++){
         // upper triangle
         glTexCoord2f(dj*j,di*i); glVertex3f(blx+j,bly+i,0); // lower left conrner
         glTexCoord2f(dj*j,di*(i+1)); glVertex3f(blx+j,bly+(i+1),0); // upper left corner
         glTexCoord2f(dj*(j+1),di*(i+1)); glVertex3f(blx+(j+1),bly+(i+1),0); // upper right corner
         // lower triangle
         glTexCoord2f(dj*j,di*i); glVertex3f(blx+j,bly+i,0); // lower left conrner
         glTexCoord2f(dj*(j+1),di*(i+1)); glVertex3f(blx+(j+1),bly+(i+1),0); // upper right corner
         glTexCoord2f(dj*(j+1),di*i); glVertex3f(blx+(j+1),bly+i,0); // lower right corner
      }
   }
   glEnd();
   glPopMatrix();
   // unbind textures
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, 0);
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, 0);
   glActiveTexture(GL_TEXTURE2);
   glBindTexture(GL_TEXTURE_2D, 0);
   glActiveTexture(GL_TEXTURE3);
   glBindTexture(GL_TEXTURE_2D, 0);
   //  Revert to fixed pipeline
   glUseProgram(0);
}

void Campground()
{
   // // forest quads
   // glActiveTexture(GL_TEXTURE0);
   // glBindTexture(GL_TEXTURE_2D, tex[4]);
   // glEnable(GL_TEXTURE_2D);
   // glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
   // glColor4f(.25,.25,.25,1);
   // glBegin(GL_QUADS);
   // glTexCoord2f(0,1); glVertex3f(-10,-10, -10);
   // glTexCoord2f(0,0); glVertex3f(10,-10, -10);
   // glTexCoord2f(1,0); glVertex3f(10,-10, 10);
   // glTexCoord2f(1,1); glVertex3f(-10,-10, 10);
   // glEnd();
   // glColor4f(.5,.5,.5,1);
   // glActiveTexture(GL_TEXTURE0);
   // glBindTexture(GL_TEXTURE_2D, tex[3]);
   // glBegin(GL_QUADS);
   // glTexCoord2f(0,0); glVertex3f(-10,-10, -10);
   // glTexCoord2f(1,0); glVertex3f(10,-10, -10);
   // glTexCoord2f(1,1); glVertex3f(10,10, -10);
   // glTexCoord2f(0,1); glVertex3f(-10,10, -10);
   // glEnd();
   // glBegin(GL_QUADS);
   // glTexCoord2f(1,0); glVertex3f(-10,-10, -10);
   // glTexCoord2f(0,0); glVertex3f(-10,-10, 10);
   // glTexCoord2f(0,1); glVertex3f(-10,10, 10);
   // glTexCoord2f(1,1); glVertex3f(-10,10, -10);
   // glEnd();
   // glColor4f(1,1,1,1);
   // // disable depth for particles
   // glDepthMask(0);
   
   // //  Integrate
   // Step();

   // //  set vars in shaders
   // glActiveTexture(GL_TEXTURE0);
   // glBindTexture(GL_TEXTURE_2D, tex[2]);
   // glUseProgram(shader[2]);
   // int id = glGetUniformLocation(shader[2],"firefly");
   // glUniform1i(id,0);
   // id = glGetUniformLocation(shader[2],"size");
   // glUniform1f(id,0.3);
   // glBlendFunc(GL_ONE,GL_ONE);
   // glEnable(GL_BLEND);

   // //  Draw points using vertex arrays
   // glEnableClientState(GL_VERTEX_ARRAY);
   // glEnableClientState(GL_COLOR_ARRAY);
   // glVertexPointer(3,GL_FLOAT,sizeof(Fly),&flys[0].x);
   // glColorPointer(3,GL_FLOAT,sizeof(Fly),&flys[0].r);
   // //  Draw all points from dst count N
   // glDrawArrays(GL_POINTS,dst,N);
   // //  Disable vertex arrays
   // glDisableClientState(GL_VERTEX_ARRAY);
   // glDisableClientState(GL_COLOR_ARRAY);
   // // disable shader and blending
   // glUseProgram(0);
   // glDisable(GL_BLEND);
   // //  reset testure and depth
   // glActiveTexture(GL_TEXTURE0);
   // glBindTexture(GL_TEXTURE_2D, 0);
   // glDepthMask(1);
   // glDisable(GL_TEXTURE_2D);
   // glColor4f(1,1,1,1);   
   terrain();
}

void anyitem()
{
   Recordplayer(0,0,0);
}

//
//  Refresh display
//
void display(GLFWwindow* window)
{
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   // set up view and proj
   Projection(fov,asp,dim);
   // camera stuff
   gluLookAt(PX,PY,PZ , PLX,PLY,PLZ , 0,1,0);
   //View(th,ph,fov,dim);
   //  Enable lighting
   Lighting(light_radius*Cos(zh),light_elv,light_radius*Sin(zh) ,l_ambient,l_diffuse,l_specular);
   
   if(mode == 0){
      Bar();
   }else if(mode == 1){
      Campground();
   }else{
      anyitem();
   }

   glDisable(GL_LIGHTING);

   //  Display parameters
   glColor3f(1,1,1);
   glWindowPos2i(5,5);
   Print("FPS=%d Angle=%d,%d zh=%d l_rad=%d l_ele=%.1f %s | gex=%d, gey=%d, gez=%d",
      FramesPerSecond(),th,ph,zh,light_radius,light_elv,text[mode],gex,gey,gez);
   
   ErrCheck("display");
   glFlush();
   glfwSwapBuffers(window);
}

//
//  Key pressed callback
//
void key(GLFWwindow* window,int key,int scancode,int action,int mods)
{
   //  Discard key releases (keeps PRESS and REPEAT)
   if (action==GLFW_RELEASE) return;

   //  Check for shift
   int shift = (mods & GLFW_MOD_SHIFT);

   //  Exit on ESC
   if (key == GLFW_KEY_ESCAPE)
      glfwSetWindowShouldClose(window,1);
   //  Reset view angle
   else if (key == GLFW_KEY_0)
      th = ph = 0;
   //  Switch shaders
   else if (key==GLFW_KEY_M)
      mode = shift ? (mode+MODE-1)%MODE : (mode+1)%MODE;
   //  Move light - y/n
   else if (key == GLFW_KEY_L)
      move_light = 1-move_light;
   //  Move light - radius
   else if (key == GLFW_KEY_LEFT_BRACKET && !shift)
      light_radius -= 1;
   else if (key == GLFW_KEY_RIGHT_BRACKET && !shift)
      light_radius += 1;
   //  Move light - height
   else if (key == GLFW_KEY_LEFT_BRACKET && shift)
      light_elv += 0.2;
   else if (key == GLFW_KEY_RIGHT_BRACKET && shift)
      light_elv -= 0.2;
   //  Right arrow key - increase angle by 5 degrees
   else if (key == GLFW_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLFW_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLFW_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLFW_KEY_DOWN)
      ph -= 5;
   // global edits
   else if (key == GLFW_KEY_W)
      gey += 1;
   else if (key == GLFW_KEY_S)
      gey -= 1;
   else if (key == GLFW_KEY_A)
      gex -= 1;
   else if (key == GLFW_KEY_D)
      gex += 1;
   else if (key == GLFW_KEY_R)
      gez -= 1;
   else if (key == GLFW_KEY_F)
      gez += 1;
   // Change camera
   double dist = .5;
   int turn_deg = 5;
   if (key == GLFW_KEY_RIGHT){
      PLX += Sin(pheta) - Sin(pheta - turn_deg);
      PLZ += Cos(pheta) - Cos(pheta - turn_deg);
      pheta -= turn_deg;
   }else if (key == GLFW_KEY_LEFT){
      PLX += Sin(pheta) - Sin(pheta + turn_deg);
      PLZ += Cos(pheta) - Cos(pheta + turn_deg);
      pheta += turn_deg;
   }else if (key == GLFW_KEY_DOWN){
      PX += dist*Sin(pheta);
      PZ += dist*Cos(pheta);
      PLX += dist*Sin(pheta);
      PLZ += dist*Cos(pheta);
   }else if (key == GLFW_KEY_UP){
      PX -= dist*Sin(pheta);
      PZ -= dist*Cos(pheta);
      PLX -= dist*Sin(pheta);
      PLZ -= dist*Cos(pheta);
   }else if (key == GLFW_KEY_SPACE && !shift){
      PY += dist;
      PLY += dist;
   }else if (key == GLFW_KEY_SPACE && shift) {
      PY -= dist;
      PLY -= dist;
   }
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Projection(fov,asp,dim);
}

//
//  Window resized callback
//
void reshape(GLFWwindow* window,int width,int height)
{
   //  Get framebuffer dimensions (makes Apple work right)
   glfwGetFramebufferSize(window,&width,&height);
   //  Ratio of the width to the height of the window
   asp = (height>0) ? width/(double)height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Update projection
   Projection(fov,asp,dim);
}

//
//  Main program with GLFW event loop
//
int main(int argc,char* argv[])
{
   //  Initialize GLFW
   GLFWwindow* window = InitWindow("Final Project: Colin Craighead & Steven Priddy",1,600,600,&reshape,&key);

   //  Load shaders
   shader[0] = CreateShaderProg("pixel.vert","phong.frag");
   shader[1] = CreateShaderProg("normmap.vert","normmap.frag");
   shader[2] = CreateShaderProgGeom("firefly.vert","firefly.geom","firefly.frag");
   shader[3] = CreateShaderProg("terrain.vert","terrain.frag");
   //  Load textures
   tex[0] = LoadTexBMP("brickwall.bmp");
   tex[1] = LoadTexBMP("brickwallnormal.bmp");
   tex[2] = LoadTexBMP("firefly.bmp");
   tex[3] = LoadTexBMP("woods.bmp");
   tex[4] = LoadTexBMP("grass.bmp");
   tex[5] = LoadTexBMP("wood.bmp");
   tex[6] = LoadTexBMP("woodnormal.bmp");
   tex[7] = LoadTexBMP("characterrecord.bmp");
   tex[8] = LoadTexBMP("recordnormal.bmp");
   tex[9] = LoadTexBMP("terrheight2.bmp");
   tex[10] = LoadTexBMP("terrnormal3.bmp");
   tex[11] = LoadTexBMP("greengrass.bmp");
   tex[12] = LoadTexBMP("darkwood.bmp");

   //  Initialize flys
   InitLoc();

   //  Event loop
   ErrCheck("init");
   while(!glfwWindowShouldClose(window))
   {
      if(move_light) zh = fmod(90*glfwGetTime(),360);
      //  Display
      display(window);
      //  Process any events
      glfwPollEvents();
   }
   //  Shut down GLFW
   glfwDestroyWindow(window);
   glfwTerminate();
   return 0;
}
