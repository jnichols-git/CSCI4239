//  Per-vertex Phong lighting
//  Vertex shader
#version 120

//  Light propeties
uniform float fov;
uniform vec4 Global;
uniform vec4 Ambient;
uniform vec4 Diffuse;
uniform vec4 Specular;
uniform vec4 Position;

vec4 phong()
{
   //  P is the vertex coordinate on body
   vec3 P = vec3(gl_ModelViewMatrix * gl_Vertex);
   //  N is the object normal at P
   vec3 N = normalize(gl_NormalMatrix * gl_Normal);
   //  L is the light vector
   vec3 L = normalize(vec3(gl_LightSource[0].position) - P);

   //  Emission and ambient color
   vec4 color = (Global+Ambient)*gl_Color;

   //  Diffuse light intensity is cosine of light and normal vectors
   float Id = dot(L,N);
   if (Id>0.0)
   {
      //  Add diffuse - material color from gl_Color
      color += Id*Diffuse*gl_Color;
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
   gl_FrontColor = phong();
   //  Pass texture coordinates to fragment shader (will be interpolated)
   gl_TexCoord[0] = gl_MultiTexCoord0;
   //  Set transformed vertex location
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
