//  Fireworks!!
//  derived from Confetti shader
#version 120

uniform   float time;  //  Time
uniform int flag;
uniform int xF;
uniform int yF;
uniform int zF;
attribute float Start; //  Start time
attribute vec3  Vel;   //  Velocity
attribute float Dur; // firework explosion duration
attribute float Vel_init; // firework explosion duration


void main(void)
{

   //  Particle location
   vec4 vert = gl_Vertex;
   //  Time offset mod 5
   float t = mod(time,5.0)-Start;
   float timeApog = sqrt((vert.y-4.5)/(0.5*9.81));
   //  Pre-launch color black
   if (t<0.0)
      vert = vec4(0,-20,0,0);
   else
   { //Draws initial particle and explosion
      //  Particle color
      gl_FrontColor = gl_Color;
      //  Particle trajectory
      vert   += t*vec4(Vel,0);
      //  Gravity
      vert.y -= 4.9*t*t;

      float distance = abs((vert.x*vert.x+vert.y*vert.y+vert.z*vert.z)-(xF*xF+yF*yF+zF*zF));

      //if(distance>5 && t>timeApog){
      //   gl_FrontColor = gl_FrontColor-(distance-5)/3.0*gl_FrontColor;
      //}


      float x = Vel_init;

      // Explosion lasts 0.3 seconds below
      if(t>(timeApog+Dur)){
         vert = vec4(0,-20,0,0);
      }
      
   
   }
   //  Transform particle location
   gl_Position = gl_ModelViewProjectionMatrix*vert;
}
