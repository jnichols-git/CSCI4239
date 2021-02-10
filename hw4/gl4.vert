#version 400 core

//  Transformation matrices
uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;

//  Light propeties
uniform float fov;
uniform vec4 Global;
uniform vec4 Ambient;
uniform vec4 Diffuse;
uniform vec4 Specular;
uniform vec4 Position;

//  Vertex attributes (input)
layout(location = 0) in vec4 Vertex;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec4 Color;
layout(location = 3) in vec2 Texture;

//  Output to next shader
out VS_GS_INTERFACE {
  out vec3 vNrm; // Normal
  out vec4 vCol; // Color
  out vec2 vTex; // TexCoord
  out mat4 vMVP; // ModelViewProjection
} vs_out;

//out vec4 FrontColor;
//out vec2 TexCoord;


vec4 phong()
{
   //  P is the vertex coordinate on body
   vec3 P = vec3(ModelViewMatrix * Vertex);
   //  N is the object normal at P
   vec3 N = normalize(NormalMatrix * Normal);
   //  L is the light vector
   vec3 L = normalize(vec3(ViewMatrix*Position) - P);

   //  Emission and ambient color
   vec4 color = (Global+Ambient)*Color;

   //  Diffuse light intensity is cosine of light and normal vectors
   float Id = dot(L,N);
   if (Id>0.0)
   {
      //  Add diffuse - material color from Color
      color += Id*Diffuse*Color;
      //  R is the reflected light vector R = 2(L.N)N - L
      vec3 R = reflect(-L, N);
      //  V is the view vector (eye at the origin)
      vec3 V = (fov>0.0) ? normalize(-P) : vec3(0,0,1);
      //  Specular is cosine of reflected and view vectors
      //  Assert material specular color is white
      float Is = dot(R,V);
      if (Is>0.0) color += pow(Is,8.0)*Specular;
   }

   //  Return sum of color components
   return color;
}


void main()
{
   //  Pass colors to fragment shader (will be interpolated)
   //FrontColor = phong();
   //  Pass texture coordinates to fragment shader (will be interpolated)
   //TexCoord = Texture;
   //  Set transformed vertex location
   gl_Position = Vertex;
   // Set stuff to go to the Geometry Shader
   vs_out.vNrm = normalize(NormalMatrix * Normal);
   vs_out.vCol = phong();
   vs_out.vTex = Texture;
   vs_out.vMVP = ProjectionMatrix * ModelViewMatrix;
}
