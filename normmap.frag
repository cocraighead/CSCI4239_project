// Normap frag shader
#version 120

varying vec3 View;
varying vec3 Light;
varying vec4 Ambient;
uniform mat3 NormalMatrix;
uniform sampler2D tex;
uniform sampler2D norms;

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

   //  Return sum of color components
   return color;
}

void main()
{
   gl_FragColor = phong() * texture2D(tex,gl_TexCoord[0].xy);
}
