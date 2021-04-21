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
int shader[] = {0,0,0,0,0,0,0};  //  Shaders
// light globals
int move_light = 1;      // if the light will be moving
int zh=0;                //  Light angle
int light_radius = 3;    // light radius 
float light_elv=1.1;     //  Light elevation
float l_ambient =0.3;    // Light properties
float l_diffuse = 0.5;
float l_specular = 0.8;
// Texture gloabls
int tex[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};     //  Textures
// Firefly globals
int n;
int N_fly=128;       //  Number of bodies
int src=0;        //  Offset of first fly in source
int dst=0;        //  Offset of first fly in destination
double vel_craig=0.04;   //  Relative speed
int ff_x = 50;
int ff_y = 47;
int ff_z = -52;
int box_size = 8; // fire flys stuck in a box

// Firework Globals
//  Set up attribute array indexes for program
#define VELOCITY_ARRAY   4
#define START_ARRAY 5
#define DUR_ARRAY 6
#define INIT_VEL_ARRAY 7
static char* Name[] = { "","","","","Vel","Start","Dur","Vel_init",NULL };
//  Point arrays
#define N 25
int fire_N = 10;
float Vert[3 * N * N];
float Color[3 * N * N];
float Vel[3 * N * N];
float Start[N * N];
float Dur[N * N];
float Vel_init[N * N];
int flag = 0;
//
//  Random numbers with range and offset
//
static float frand(float rng, float off)
{
    return rand() * rng / RAND_MAX + off;
}
// Velocities of particle
float xVel[10], yVel[10], zVel[10];
// Different firework y velocities
float timeApog; // Time at which the initial particle reaches apogee and explodes
int explodeFlag = 1;
int partCounter = 0;
// Arrays for fireworks
float x0[10] = {-35,-25,-15,-5,5,15,25,30,45,50 };
float y0_sos[10] = { 25,25,25,25,25,25,25,25,25,25 };
float z0[10] =  { -35,-25,-15,-5,5,15,25,30,45,50 };
float startTime[10] = { 0,2,5,6,9,10,13,15,17,17 };
float yVel_0[10] = { 30,35,40,30,35,40,30,35,40,30 };
//  Array Pointers
float* vert = Vert;
float* color = Color;
float* vel = Vel;
float* start = Start;
float* duration = Dur;
float* vel_init = Vel_init;

// First phase of firework particle shooting up
void firework_init(int i, float r1, float r2, float g1, float g2, float b1, float b2) {
    //  Location x,y,z
    *vert++ = x0[i];
    *vert++ = y0_sos[i];
    *vert++ = z0[i];
    //  Color r,g,b (0.5-1.0)
    *color++ = frand(r1, r2);
    *color++ = frand(g1, g2);
    *color++ = frand(b1, b2);

    // Kinematics for tracking start of explosion 
    xVel[i] = 1.0 * frand(0.5, 0.5);
    yVel[i] = yVel_0[i];
    zVel[i] = 1.0 * frand(0.5, 0.5);

    //  Velocity
    *vel++ = xVel[i];
    *vel++ = yVel[i];
    *vel++ = zVel[i];

    //  Launch time
    *start++ = startTime[i];
    // Duration
    *duration++ = 0.0;
    // Initial particle velocity
    *vel_init++ = yVel[i];
    // The time at which the initial particle will reach apogee
    timeApog = yVel[i] / 9.8;
}

// Explosion phase for fireworks
void firework_exp(int i, float r1, float r2, float g1, float g2, float b1, float b2, float vx, float vy, float vz, float dur) {
    timeApog = yVel[i] / 9.8;
    *vert++ = x0[i] + xVel[i] * timeApog;
    *vert++ = y0_sos[i] + yVel[i] * timeApog - 0.5 * 9.8 * timeApog * timeApog;
    *vert++ = z0[i] + zVel[i] * timeApog;
    //  Color r,g,b (0.5-1.0)
    *color++ = frand(r1, r2);
    *color++ = frand(g1, g2);
    *color++ = frand(b1, b2);
    // Duration
    *duration++ = dur;
    // Initial particle velocity
    *vel_init++ = 0;

    //  Velocity
    float ang = (rand()) / ((RAND_MAX / 7)); // random float between 0 and 7
    *vel++ = frand(vx * cos(ang), 0);
    *vel++ = frand(vy, 0);
    *vel++ = frand(vz * sin(ang), 0);
    //  Launch time
    *start++ = startTime[i] + timeApog + frand(0.5, 0);
}


//
//  Initialize particles for firework shader
//
void InitPart(void)
{
    //  Loop over NxN patch
    int i, j;
    n = N;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++)
        {
            // Beginning of  firework 1 
            if (i == 0 && j == 0) {
                firework_init(0, 0.5, 0.5, 0.25, 0.75, 0.25, 0.725);
            // Beginning of  firework 2
            }
            else if (i == 1 && j == 1) {
                firework_init(1, 0.25, 0.75, 0.25, 0.5, 0.25, 0.5);
            }
            // Beginning of  firework 3
            else if (i == 2 && j == 2) {
                firework_init(2, 0.25, 0.75, 0.25, 0.5, 0.25, 0.5);
            }
            // Beginning of  firework 4
            else if (i == 3 && j == 3) {
                firework_init(3, 0.25, 0.75, 0.25, 0.5, 0.25, 0.5);
            }
            // Beginning of  firework 5
            else if (i == 4 && j == 4) {
                firework_init(4, 0.25, 0.75, 0.25, 0.5, 0.25, 0.5);
            }
            // Beginning of  firework 6
            else if (i == 5 && j == 5) {
                firework_init(5, 0.25, 0.75, 0.25, 0.5, 0.25, 0.5);
            }
            // Beginning of  firework 7
            else if (i == 6 && j == 6) {
                firework_init(6, 0.25, 0.75, 0.25, 0.5, 0.25, 0.5);
            }
            // Beginning of  firework 8
            else if (i == 7 && j == 7) {
                firework_init(7, 0.25, 0.75, 0.25, 0.5, 0.25, 0.5);
            }
            // Beginning of  firework 9
            else if (i == 8 && j == 8) {
            firework_init(8, 0.25, 0.75, 0.25, 0.5, 0.25, 0.5);
            }
            // Beginning of  firework 10
            else if (i == 9 && j == 9) {
            firework_init(9, 0.25, 0.75, 0.25, 0.5, 0.25, 0.5);
            }

            else { // Firework Explosion

            //  Location x,y,z using kinematics (ignoring drag) -> start where initial rocket ends at
                if (partCounter < round(n * n / 10.0)) {
                    firework_exp(0, 0.2, 0.5, 0.25, 0.75, 0.5, 0.5, 15.0, 20.0, 15.0, 1.2);
                }
                else if ( (partCounter > round(n * n / 10.0)) && (partCounter < round(2*n * n / 10.0))) {
                    firework_exp(1, 0.2, 0.5, 0.25, 0.75, 0.5, 0.5, 15.0, 20.0, 15.0, 1.2);
                }
                else if ((partCounter > round(2*n * n / 10.0)) && (partCounter < round(3 * n * n / 10.0))) {
                    firework_exp(2, 0.2, 0.5, 0.25, 0.75, 0.5, 0.5, 15.0, 20.0, 15.0, 1.2);
                }
                else if ((partCounter > round(3*n * n / 10.0)) && (partCounter < round(4 * n * n / 10.0))) {
                    firework_exp(3, 0.2, 0.5, 0.25, 0.75, 0.5, 0.5, 15.0, 20.0, 15.0, 1.2);
                }
                else if ((partCounter > round(4*n * n / 10.0)) && (partCounter < round(5 * n * n / 10.0))) {
                    firework_exp(4, 0.2, 0.5, 0.25, 0.75, 0.5, 0.5, 15.0, 20.0, 15.0, 1.2);
                }
                else if ((partCounter > round(5*n * n / 10.0)) && (partCounter < round(6 * n * n / 10.0))) {
                    firework_exp(5, 0.2, 0.5, 0.25, 0.75, 0.5, 0.5, 15.0, 20.0, 15.0, 1.2);
                }
                else if ((partCounter > round(6*n * n / 10.0)) && (partCounter < round(7 * n * n / 10.0))) {
                    firework_exp(6, 0.2, 0.5, 0.25, 0.75, 0.5, 0.5, 15.0, 20.0, 15.0, 1.2);
                }
                else if ((partCounter > round(7*n * n / 10.0)) && (partCounter < round(8 * n * n / 10.0))) {
                    firework_exp(7, 0.2, 0.5, 0.25, 0.75, 0.5, 0.5, 15.0, 20.0, 15.0, 1.2);
                }
                else if ((partCounter > round(8*n * n / 10.0)) && (partCounter < round(9 * n * n / 10.0))) {
                    firework_exp(8, 0.2, 0.5, 0.25, 0.75, 0.5, 0.5, 15.0, 20.0, 15.0, 1.2);
                }
                else {
                    firework_exp(9, 0.5, 0.5, 0.25, 0.25, 0.25, 0.25, 20.0, 25.0, 20.0, 1.0);
                }
                partCounter++;
            }
        }
    }
}

//
//  Draw particles for fireworks
//
void DrawPart(void)
{
    //  Set particle size
    glPointSize(3);
    //  Point vertex location to local array Vert
    glVertexPointer(3, GL_FLOAT, 0, Vert);
    //  Point color array to local array Color
    glColorPointer(3, GL_FLOAT, 0, Color);
    //  Point attribute arrays to local arrays
    glVertexAttribPointer(VELOCITY_ARRAY, 3, GL_FLOAT, GL_FALSE, 0, Vel);
    glVertexAttribPointer(START_ARRAY, 1, GL_FLOAT, GL_FALSE, 0, Start);
    glVertexAttribPointer(DUR_ARRAY, 1, GL_FLOAT, GL_FALSE, 0, Dur);
    glVertexAttribPointer(INIT_VEL_ARRAY, 1, GL_FLOAT, GL_FALSE, 0, Vel_init);

    //  Enable arrays used by DrawArrays
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableVertexAttribArray(VELOCITY_ARRAY);
    glEnableVertexAttribArray(START_ARRAY);
    glEnableVertexAttribArray(DUR_ARRAY);
    glEnableVertexAttribArray(INIT_VEL_ARRAY);
    //  Set transparent large particles
    if (mode)
    {
        glEnable(GL_POINT_SPRITE);
        glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDepthMask(0);
    }
    //  Draw arrays
    glDrawArrays(GL_POINTS, 0, n * n);
    //  Reset
    if (mode)
    {
        glDisable(GL_POINT_SPRITE);
        glDisable(GL_BLEND);
        glDepthMask(1);
    }
    //  Disable arrays
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableVertexAttribArray(VELOCITY_ARRAY);
    glDisableVertexAttribArray(START_ARRAY);
    glDisableVertexAttribArray(DUR_ARRAY);
    glDisableVertexAttribArray(INIT_VEL_ARRAY);
}



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

// lines that make up neon sign
float neonsignarr[] = {-1.0,0.0,0.0 ,0.0,1.0,1.0,
                       -.812,.172,0.0 ,0.0,1.0,1.0, // line 1 -- Blue Mountains
                       -.509,.535,0.0 ,0.0,1.0,1.0,
                       -.407,.641,0.0 ,0.0,1.0,1.0, // line 2
                       -.419,.641,0.0 ,0.0,1.0,1.0,
                       -.354,.579,0.0 ,0.0,1.0,1.0, // line 3
                       -.367,.580,0.0 ,0.0,1.0,1.0,
                       -.121,.809,0.0 ,0.0,1.0,1.0, // line 4
                       -.131,.808,0.0 ,0.0,1.0,1.0,
                        .007,.695,0.0 ,0.0,1.0,1.0, // line 5
                       -.005,.695,0.0 ,0.0,1.0,1.0,
                        .035,.735,0.0 ,0.0,1.0,1.0, // line 6
                        .024,.734,0.0 ,0.0,1.0,1.0,
                        .213,.594,0.0 ,0.0,1.0,1.0, // line 7
                        .201,.594,0.0 ,0.0,1.0,1.0,
                        .242,.638,0.0 ,0.0,1.0,1.0, // line 8
                        .230,.636,0.0 ,0.0,1.0,1.0,
                        .691,.300,0.0 ,0.0,1.0,1.0, // line 9
                       -.808,.608,0.0 ,1.0,.455,.557,
                       -.797,.539,0.0 ,1.0,.455,.557, // line 1 -- Red C
                       -.798,.541,0.0 ,1.0,.455,.557,
                       -.752,.471,0.0 ,1.0,.455,.557, // line 2
                       -.764,.473,0.0 ,1.0,.455,.557,
                       -.657,.508,0.0 ,1.0,.455,.557, // line 3
                       -.673,.498,0.0 ,1.0,.455,.557,
                       -.630,.566,0.0 ,1.0,.455,.557, // line 4
                       -.675,.513,0.0 ,1.0,.455,.557,
                       -.628,.465,0.0 ,1.0,.455,.557, // line 5
                       -.639,.472,0.0 ,1.0,.455,.557,
                       -.570,.474,0.0 ,1.0,.455,.557, // line 6
                       -.585,.469,0.0 ,1.0,.455,.557,
                       -.520,.564,0.0 ,1.0,.455,.557, // line 7
                       -.521,.556,0.0 ,1.0,.455,.557,
                       -.533,.640,0.0 ,1.0,.455,.557, // line 8
                       -.535,.636,0.0 ,1.0,.455,.557,
                       -.607,.670,0.0 ,1.0,.455,.557, // line 9
                       -.594,.671,0.0 ,1.0,.455,.557,
                       -.665,.634,0.0 ,1.0,.455,.557, // line 10
                       -.658,.639,0.0 ,1.0,.455,.557,
                       -.727,.570,0.0 ,1.0,.455,.557, // line 11
                       -.725,.574,0.0 ,1.0,.455,.557,
                       -.795,.452,0.0 ,1.0,.455,.557, // line 12
                       -.794,.456,0.0 ,1.0,.455,.557,
                       -.817,.284,0.0 ,1.0,.455,.557, // line 13
                       -.817,.289,0.0 ,1.0,.455,.557,
                       -.803,.191,0.0 ,1.0,.455,.557, // line 14
                       -.802,.189,0.0 ,1.0,.455,.557,
                       -.703,.125,0.0 ,1.0,.455,.557, // line 15
                       -.707,.126,0.0 ,1.0,.455,.557,
                       -.586,.135,0.0 ,1.0,.455,.557, // line 16
                       -.594,.133,0.0 ,1.0,.455,.557,
                       -.500,.213,0.0 ,1.0,.455,.557, // line 17
                       -.329,.434,0.0 ,1.0,.455,.557,
                       -.421,.391,0.0 ,1.0,.455,.557, // line 1 - Red o
                       -.417,.395,0.0 ,1.0,.455,.557,
                       -.468,.300,0.0 ,1.0,.455,.557, // line 2
                       -.469,.300,0.0 ,1.0,.455,.557,
                       -.486,.211,0.0 ,1.0,.455,.557, // line 3
                       -.492,.217,0.0 ,1.0,.455,.557,
                       -.428,.140,0.0 ,1.0,.455,.557, // line 4
                       -.434,.147,0.0 ,1.0,.455,.557,
                       -.320,.136,0.0 ,1.0,.455,.557, // line 5
                       -.329,.132,0.0 ,1.0,.455,.557,
                       -.235,.213,0.0 ,1.0,.455,.557, // line 6
                       -.240,.207,0.0 ,1.0,.455,.557,
                       -.195,.314,0.0 ,1.0,.455,.557, // line 7
                       -.193,.306,0.0 ,1.0,.455,.557,
                       -.236,.398,0.0 ,1.0,.455,.557, // line 8
                       -.236,.394,0.0 ,1.0,.455,.557,
                       -.303,.424,0.0 ,1.0,.455,.557, // line 9
                       -.301,.426,0.0 ,1.0,.455,.557,
                       -.315,.346,0.0 ,1.0,.455,.557, // line 10
                       -.317,.354,0.0 ,1.0,.455,.557,
                       -.270,.309,0.0 ,1.0,.455,.557, // line 11
                       -.279,.312,0.0 ,1.0,.455,.557,
                       -.169,.348,0.0 ,1.0,.455,.557, // line 12
                         0.0,.434,0.0 ,1.0,.455,.557,
                       -.092,.391,0.0 ,1.0,.455,.557, // line 1 - Red o #2
                       -.088,.395,0.0 ,1.0,.455,.557,
                       -.139,.300,0.0 ,1.0,.455,.557, // line 2
                       -.140,.300,0.0 ,1.0,.455,.557,
                       -.157,.211,0.0 ,1.0,.455,.557, // line 3
                       -.163,.217,0.0 ,1.0,.455,.557,
                       -.099,.140,0.0 ,1.0,.455,.557, // line 4
                       -.105,.147,0.0 ,1.0,.455,.557,
                        .009,.136,0.0 ,1.0,.455,.557, // line 5
                         0.0,.132,0.0 ,1.0,.455,.557,
                        .094,.213,0.0 ,1.0,.455,.557, // line 6
                        .089,.207,0.0 ,1.0,.455,.557,
                        .134,.314,0.0 ,1.0,.455,.557, // line 7
                        .136,.306,0.0 ,1.0,.455,.557,
                        .093,.398,0.0 ,1.0,.455,.557, // line 8
                        .093,.394,0.0 ,1.0,.455,.557,
                        .026,.424,0.0 ,1.0,.455,.557, // line 9
                        .028,.426,0.0 ,1.0,.455,.557,
                        .014,.346,0.0 ,1.0,.455,.557, // line 10
                        .012,.354,0.0 ,1.0,.455,.557,
                        .059,.309,0.0 ,1.0,.455,.557, // line 11
                        .050,.312,0.0 ,1.0,.455,.557,
                        .160,.348,0.0 ,1.0,.455,.557, // line 12
                        .158,.347,0.0 ,1.0,.455,.557,
                        .236,.395,0.0 ,1.0,.455,.557, // line 1 - red r
                        .230,.392,0.0 ,1.0,.455,.557,
                        .247,.457,0.0 ,1.0,.455,.557, // line 2
                        .218,.388,0.0 ,1.0,.455,.557,
                        .287,.325,0.0 ,1.0,.455,.557, // line 3
                        .283,.333,0.0 ,1.0,.455,.557,
                        .255,.223,0.0 ,1.0,.455,.557, // line 4
                        .256,.236,0.0 ,1.0,.455,.557,
                        .161,.166,0.0 ,1.0,.455,.557, // line 5
                        .169,.175,0.0 ,1.0,.455,.557,
                        .173,.119,0.0 ,1.0,.455,.557, // line 6
                        .168,.124,0.0 ,1.0,.455,.557,
                        .236,.124,0.0 ,1.0,.455,.557, // line 7
                        .224,.122,0.0 ,1.0,.455,.557,
                        .378,.246,0.0 ,1.0,.455,.557, // line 8
                        .375,.240,0.0 ,1.0,.455,.557,
                        .423,.354,0.0 ,1.0,.455,.557, // line 1 - red s
                        .419,.349,0.0 ,1.0,.455,.557,
                        .466,.387,0.0 ,1.0,.455,.557, // line 2
                        .433,.338,0.0 ,1.0,.455,.557,
                        .480,.256,0.0 ,1.0,.455,.557, // line 3
                        .480,.259,0.0 ,1.0,.455,.557,
                        .489,.159,0.0 ,1.0,.455,.557, // line 4
                        .491,.166,0.0 ,1.0,.455,.557,
                        .452,.110,0.0 ,1.0,.455,.557, // line 5
                        .461,.112,0.0 ,1.0,.455,.557,
                        .382,.122,0.0 ,1.0,.455,.557, // line 6
                        .387,.117,0.0 ,1.0,.455,.557,
                        .375,.178,0.0 ,1.0,.455,.557, // line 7
                        .378,.170,0.0 ,1.0,.455,.557,
                        .394,.216,0.0 ,1.0,.455,.557, // line 8
                        .487,.160,0.0 ,1.0,.455,.557,
                        .563,.192,0.0 ,1.0,.455,.557, // line 9
                        .553,.190,0.0 ,1.0,.455,.557,
                        .601,.162,0.0 ,1.0,.455,.557, // line 10
                       };

void neon_sign(float x, float y, float z, float s){
   glPushMatrix();
   // transformations
   glTranslated(x,y,z);
   glScaled(s,s,1);
   // geo shader
   glUseProgram(shader[4]);
   int id = glGetUniformLocation(shader[4],"r");
   glUniform1f(id,0.01);
   //  Draw points using vertex arrays
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_COLOR_ARRAY);
   glVertexPointer(3,GL_FLOAT,24,&neonsignarr[0]);
   glColorPointer(3,GL_FLOAT,24,&neonsignarr[3]);
   //  Draw all lines
   glDrawArrays(GL_LINES,0,136);
   //  Disable vertex arrays
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_COLOR_ARRAY);
   // disable shader and blending
   glPopMatrix();
   glUseProgram(0);
}


void sky(float D, int sidetex, int topbottex)
{
   glColor3f(1,1,1);
   glActiveTexture(GL_TEXTURE0);
   glEnable(GL_TEXTURE_2D);

   //  Sides
   glBindTexture(GL_TEXTURE_2D,sidetex);
   glBegin(GL_QUADS);
   glTexCoord2f(0.00,0); glVertex3f(-D,-D,-D);
   glTexCoord2f(0.25,0); glVertex3f(+D,-D,-D);
   glTexCoord2f(0.25,1); glVertex3f(+D,+D,-D);
   glTexCoord2f(0.00,1); glVertex3f(-D,+D,-D);

   glTexCoord2f(0.25,0); glVertex3f(+D,-D,-D);
   glTexCoord2f(0.50,0); glVertex3f(+D,-D,+D);
   glTexCoord2f(0.50,1); glVertex3f(+D,+D,+D);
   glTexCoord2f(0.25,1); glVertex3f(+D,+D,-D);

   glTexCoord2f(0.50,0); glVertex3f(+D,-D,+D);
   glTexCoord2f(0.75,0); glVertex3f(-D,-D,+D);
   glTexCoord2f(0.75,1); glVertex3f(-D,+D,+D);
   glTexCoord2f(0.50,1); glVertex3f(+D,+D,+D);

   glTexCoord2f(0.75,0); glVertex3f(-D,-D,+D);
   glTexCoord2f(1.00,0); glVertex3f(-D,-D,-D);
   glTexCoord2f(1.00,1); glVertex3f(-D,+D,-D);
   glTexCoord2f(0.75,1); glVertex3f(-D,+D,+D);
   glEnd();

   //  Top and bottom
   glBindTexture(GL_TEXTURE_2D,topbottex);
   glBegin(GL_QUADS);
   glTexCoord2f(0.0,0); glVertex3f(+D,+D,-D);
   glTexCoord2f(0.5,0); glVertex3f(+D,+D,+D);
   glTexCoord2f(0.5,1); glVertex3f(-D,+D,+D);
   glTexCoord2f(0.0,1); glVertex3f(-D,+D,-D);

   glTexCoord2f(1.0,1); glVertex3f(-D,-D,+D);
   glTexCoord2f(0.5,1); glVertex3f(+D,-D,+D);
   glTexCoord2f(0.5,0); glVertex3f(+D,-D,-D);
   glTexCoord2f(1.0,0); glVertex3f(-D,-D,-D);
   glEnd();

   // unbind 
   glDisable(GL_TEXTURE_2D);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, 0);

}

void water_normmap_quad(float x, float y, float z, float dx, float dy, float dz, float rx, float ry, float rz, int image_tex, int norm_tex)
{
   //  Select shader
   glUseProgram(shader[1]);
   //  Walls material and color
   float color[][4] = {{0,0,0,0},{.5,.5,.5,0},{1,1,0,0},{0,1,0,0},{0,1,1,0},{0,0,1,0},{1,0,1,0},{1,1,1,1}};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,96.0);
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
   glTexCoord2f(0,0); glVertex3f(-1*dx,-1*dy, 0);
   glTexCoord2f(1*dx,0); glVertex3f(+1*dx,-1*dy, 0);
   glTexCoord2f(1*dx,1*dy); glVertex3f(+1*dx,+1*dy, 0);
   glTexCoord2f(0,1*dy); glVertex3f(-1*dx,+1*dy, 0);
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

void neonsign_normmap_quad(float x, float y, float z, float dx, float dy, float dz, float rx, float ry, float rz, int image_tex, int norm_tex, float lx, float ly)
{
   //  Select shader
   glUseProgram(shader[5]);
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
   int id = glGetUniformLocation(shader[5],"tex");
   glUniform1i(id,0);
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, norm_tex);
   id = glGetUniformLocation(shader[5],"norms");
   glUniform1i(id,1);
   // neon uniforms
   id = glGetUniformLocation(shader[5],"neon_light_tp");
   glUniform2f(id,lx,ly);
   id = glGetUniformLocation(shader[5],"neon_light_color");
   glUniform3f(id,1.0,.1,.4);
   id = glGetUniformLocation(shader[5],"r");
   glUniform1f(id,1.6);
   // cube front face
   // save transformation
   glPushMatrix();
   // transformations
   glTranslated(x,y,z);
   glRotated(ry,0,1,0);
   glScaled(dx,dy,1);
   // get current model view matrix
   float modelview_matrix[16]; 
   glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix); 
   // get Normal matrix
   float normal_matrix[9];
   mat4normalMatrix(modelview_matrix, normal_matrix);
   id = glGetUniformLocation(shader[5],"NormalMatrix");
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
void record_player(int x, int y, int z)
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

void picktable(int x, int y, int z)
{
   // table top
   Cube(x+-1.25,y+2.3,z+0, .25,.1,2.5, 0,0, tex[5]);
   Cube(x+-.75,y+2.3,z+0, .25,.1,2.5, 0,0, tex[5]);
   Cube(x+-.25,y+2.3,z+0, .25,.1,2.5, 0,0, tex[5]);
   Cube(x+.25,y+2.3,z+0, .25,.1,2.5, 0,0, tex[5]);
   Cube(x+.75,y+2.3,z+0, .25,.1,2.5, 0,0, tex[5]);
   Cube(x+1.25,y+2.3,z+0, .25,.1,2.5, 0,0, tex[5]);
   // seats
   Cube(x+2.2,y+1.2,z+0, .5,.1,2.5, 0,0, tex[5]);
   Cube(x+-2.2,y+1.2,z+0, .5,.1,2.5, 0,0, tex[5]);
   // seat holder
   Cube(x+0,y+.8,z+2.2, 2.7,.3,.1, 0,0, tex[5]);
   Cube(x+0,y+.8,z+-2.2, 2.7,.3,.1, 0,0, tex[5]);
   // legs
   Cube(x+2,y+.9,z+2.1, .2,.1,1.5, -30,-90, tex[5]);
   Cube(x+-2,y+.9,z+2.1, .2,.1,1.5, 30,-90, tex[5]);
   Cube(x+2,y+.9,z+-2.1, .2,.1,1.5, -30,-90, tex[5]);
   Cube(x+-2,y+.9,z+-2.1, .2,.1,1.5, 30,-90, tex[5]);
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
         if( (i != 2 || j != 5)&&(i != 2 || j != 6)&&(i != 1 || j != 5)&&(i != 1 || j != 6) ){ // sign wall - minus 4 quads
            normmap_quad(4*j-22,4*i+1,-16,2,2,1,0,0,0,tex[0],tex[1]);
         }
      }
   }
   // sign quads
   neonsign_normmap_quad(-2,9,-16,2,2,1,0,0,0,tex[0],tex[1],1.0,0.3);
   neonsign_normmap_quad(2,9,-16,2,2,1,0,0,0,tex[0],tex[1],0.0,0.3);
   neonsign_normmap_quad(-2,5,-16,2,2,1,0,0,0,tex[0],tex[1],1.0,1.3);
   neonsign_normmap_quad(2,5,-16,2,2,1,0,0,0,tex[0],tex[1],0.0,1.3);
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
   record_player(-18,5,-8);
   // neon sign
   neon_sign(.4,7.8,-15.85,2.0);
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
   float ax = ((rand()%1000)-500)/500.0;
   float ay = ((rand()%1000)-500)/500.0;
   float az = ((rand()%1000)-500)/500.0;
   //  Update velocity
   // flip vel if fly is close to box edge
   if(flys[k0].x > ff_x+box_size || flys[k0].x < ff_x-box_size){
      flys[k0].u *= -1;
   }
   if(flys[k0].y > ff_y+box_size || flys[k0].y < ff_y-box_size){
      flys[k0].v *= -1;
   }
   if(flys[k0].z > ff_z+box_size || flys[k0].z < ff_z-box_size){
      flys[k0].w *= -1;
   }
   // speed limit
   if(abs(flys[k0].u) > 13.0){
      flys[k0].u /= 2.0;
   }
   if(abs(flys[k0].v) > 13.0){
      flys[k0].v /= 2.0;
   }
   if(abs(flys[k0].w) > 13.0){
      flys[k0].w /= 2.0;
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
   src = src?0:N_fly;
   dst = dst?0:N_fly;
   for (k=0;k<N_fly;k++)
      Move(k);
}

//
//  random value for the location of the flies
//
void rand_loc(float* X,float* Y,float* Z)
{
   *X = (rand()%(20*box_size) - 10*box_size)/10.5 + ff_x;
   *Y = (rand()%(20*box_size) - 10*box_size)/10.5 + ff_y;
   *Z = (rand()%(20*box_size) - 10*box_size)/10.5 + ff_z;
}


//
//  Scaled random value for the velocity of the flies
//
void rand_vel(float Sx,float Sy,float Sz,float* X,float* Y,float* Z)
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
   if (!flys) flys = malloc(2*N_fly*sizeof(Fly));
   if (!flys) Fatal("Error allocating memory for %d flys\n",N_fly);
   src = N_fly;
   dst = 0;
   //  Assign random locations
   for (k=0;k<N_fly;k++)
   {
      rand_loc(&flys[k].x,&flys[k].y,&flys[k].z);
      rand_vel(vel_craig,vel_craig,vel_craig,&flys[k].u,&flys[k].v,&flys[k].w);
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
      flys[k+N_fly].r = flys[k].r;
      flys[k+N_fly].g = flys[k].g;
      flys[k+N_fly].b = flys[k].b;
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

void fireflies()
{
   // disable depth for particles
   glDepthMask(0);
   
   //  Integrate
   Step();

   //  set vars in shaders
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, tex[2]);
   glUseProgram(shader[2]);
   int id = glGetUniformLocation(shader[2],"firefly");
   glUniform1i(id,0);
   id = glGetUniformLocation(shader[2],"size");
   glUniform1f(id,0.3);
   glBlendFunc(GL_ONE,GL_ONE);
   glEnable(GL_BLEND);

   //  Draw points using vertex arrays
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_COLOR_ARRAY);
   glVertexPointer(3,GL_FLOAT,sizeof(Fly),&flys[0].x);
   glColorPointer(3,GL_FLOAT,sizeof(Fly),&flys[0].r);
   //  Draw all points from dst count N
   glDrawArrays(GL_POINTS,dst,N_fly);
   //  Disable vertex arrays
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_COLOR_ARRAY);
   // disable shader and blending
   glUseProgram(0);
   glDisable(GL_BLEND);
   //  reset testure and depth
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, 0);
   glDepthMask(1);
   glDisable(GL_TEXTURE_2D);
   glColor4f(1,1,1,1);
}

void fireworkShader() {
    glUseProgram(shader[6]);
    //  Set time
    int id = glGetUniformLocation(shader[6], "time");
    glUniform1f(id, glfwGetTime());
    id = glGetUniformLocation(shader[6], "Noise3D");
    glUniform1i(id, 1);
    id = glGetUniformLocation(shader[6], "img");
    glUniform1i(id, 0);
    id = glGetUniformLocation(shader[6], "flag");
    glUniform1i(id, flag);

    //  Draw the particles
    DrawPart();

    //  No shader for what follows
    glUseProgram(0);

}

void Campground()
{
   // campsite platform
   Cube(50,37,-39, 14,1,1, 0,0, tex[12]);
   Cube(50,37,-65, 14,1,1, 0,0, tex[12]);
   Cube(63,37,-52, 1,1,12, 0,0, tex[12]);
   Cube(37,37,-52, 1,1,12, 0,0, tex[12]);
   Cube(50,36.5,-52, 12,1,12, 0,0, tex[4]);
   // Table
   picktable(55,38,-57);
   // lake
   water_normmap_quad(11,-2,90, 50,50,1, -90,0,0, tex[16],tex[16]);
   // terrain
   terrain();

}

void anyitem()
{
   //record_player(0,0,0);
   // water_normmap_quad(0,0,0, 4,2,1, -90,0,0, tex[16],tex[16]);
   neon_sign(0,0,0,1);
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
   if(mode == 1){
      sky(268,tex[13],tex[14]);
   }
   // firefly particles
   if(mode == 1){
      fireflies();
      fireworkShader();
   }



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
   shader[4] = CreateShaderProgGeom("neonsign.vert","neonsign.geom","neonsign.frag");
   shader[5] = CreateShaderProg("neon_normmap.vert","neon_normmap.frag");
   shader[6] = CreateShaderProgAttr("firework.vert", NULL, Name);
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
   tex[13] = LoadTexBMP("nightskybox1.bmp");
   tex[14] = LoadTexBMP("nightskybox2.bmp");
   tex[15] = LoadTexBMP("starwater.bmp");
   tex[16] = LoadTexBMP("waternormal.bmp");

   //  Initialize flys
   InitLoc();
   //  Initialize particles
   InitPart();

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
