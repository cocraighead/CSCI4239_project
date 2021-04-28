//  Fragment shader - Lighting on normap quad behind neon sign
#version 120

varying vec3 View;
varying vec3 Light;
varying vec4 Ambient;
uniform vec3 neon_light_color;
uniform vec2 neon_light_tp;
uniform float r;
uniform mat3 NormalMatrix;
uniform sampler2D tex;
uniform sampler2D norms;

vec3 my_mix(vec3 x, vec3 y, float a){
   if(a > 1) a = 1;
   if(a < 0) a = 0;
   return mix(x,y,a);
}

vec4 phong()
{  
   // Get normal from texture
   vec3 Normal = texture2D(norms,gl_TexCoord[0].xy).rgb;
   // shift it from [0,1] to [-1,1]
   Normal = 2*Normal - 1;
   // apply Transformations to the normal
   Normal = NormalMatrix * Normal;
   //  N is the object normal
   vec3 N = normalize(Normal);
   //  L is the light vector
   vec3 L = normalize(Light);

   //  Emission and ambient color
   vec4 color = Ambient;

   //  Diffuse light is cosine of light and normal vectors
   float Id = dot(L,N);
   if (Id>0.0)
   {
      //  Add diffuse
      color += Id*gl_FrontLightProduct[0].diffuse;
      //  R is the reflected light vector R = 2(L.N)N - L
      vec3 R = reflect(-L,N);
      //  V is the view vector (eye vector)
      vec3 V = normalize(View);
      //  Specular is cosine of reflected and view vectors
      float Is = dot(R,V);
      if (Is>0.0) color += pow(Is,gl_FrontMaterial.shininess)*gl_FrontLightProduct[0].specular;
   }

   //  Return sum of color components - adds red in a radius around "sign center"
   color.rgb += my_mix(neon_light_color,vec3(0,0,0),r*length(gl_TexCoord[0].xy-neon_light_tp));
   return color;
}

void main()
{
   gl_FragColor = phong() * texture2D(tex,gl_TexCoord[0].xy);
}
