//  Per-pixel lighting Vertex shader
#version 120

varying vec3 View;
varying vec3 Light;
varying vec4 Ambient;
varying vec4 world_coord;
uniform mat4 ModelMatrix;

uniform sampler2D heights;

void main()
{
   //
   //  Lighting values needed by fragment shader
   //
   //  Texture coordinate for fragment shader
   gl_TexCoord[0] = gl_MultiTexCoord0;
   // move terrain to height map height
   vec4 risen_vertex = gl_Vertex;
   risen_vertex.z += texture2D(heights ,gl_TexCoord[0].xy).r*10;
   //  Vertex location in modelview coordinates
   vec3 P = vec3(gl_ModelViewMatrix * risen_vertex);
   //  Light position
   Light  = vec3(gl_LightSource[0].position) - P;
   //  Eye position
   View  = -P;
   //  Ambient color
   Ambient = gl_FrontMaterial.emission + gl_FrontLightProduct[0].ambient + gl_LightModel.ambient*gl_FrontMaterial.ambient;

   
   // world coordinate
   world_coord = ModelMatrix * risen_vertex;
   //  Set vertex position
   gl_Position = gl_ModelViewProjectionMatrix * risen_vertex;
}
