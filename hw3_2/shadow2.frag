//  Shadow Fragment shader
#version 120

varying vec3 View;
varying vec3 Light;
varying vec3 Normal;
varying vec4 Ambient;
uniform sampler2D tex;
uniform sampler2DShadow depth;

vec3 my_normalize(vec3 v) {
  float len = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
  return v/len;
}
float my_dot(vec3 a, vec3 b) {
  return (a.x*b.x)+(a.y*b.y)+(a.z*b.z);
}

vec4 phong()
{
   //  Emission and ambient color
   vec4 color = Ambient;

   //  Do lighting if not in the shadow
   if (shadow2DProj(depth,gl_TexCoord[1]).a==1.0)
   {
      //  N is the object normal
      vec3 N = my_normalize(Normal);
      //  L is the light vector
      vec3 L = my_normalize(Light);

      //  Diffuse light is cosine of light and normal vectors
      float Id = my_dot(L,N);
      if (Id>0.0)
      {
         //  Add diffuse
         color += Id*gl_FrontLightProduct[0].diffuse;
         //  R is the reflected light vector R = 2(L.N)N - L
         vec3 R = reflect(-L,N);
         //  V is the view vector (eye vector)
         vec3 V = my_normalize(View);
         //  Specular is cosine of reflected and view vectors
         float Is = my_dot(R,V);
         if (Is>0.0) color += pow(Is,gl_FrontMaterial.shininess)*gl_FrontLightProduct[0].specular;
      }
   }

   //  Return result
   return color;
}

void main()
{
   //  Compute pixel lighting modulated by texture
   gl_FragColor = phong() * texture2D(tex,gl_TexCoord[0].st);
}
