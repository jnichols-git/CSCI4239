#include "CSCIx239.h"
//
//  Draw a cube
//

//
//  Solid unit cube
//
void SolidCube()
{
   //  Set colors
   float color[4] = {1,1,1,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,32.0);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,color);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,color);

   //  Front
   glColor4fv(color);
   glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,color);
   glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,color);
   glNormal3f( 0, 0, 1);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
   glTexCoord2f(1,0); glVertex3f(+1,-1, 1);
   glTexCoord2f(1,1); glVertex3f(+1,+1, 1);
   glTexCoord2f(0,1); glVertex3f(-1,+1, 1);
   glEnd();

   //  Back
   glColor4fv(color);
   glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,color);
   glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,color);
   glNormal3f( 0, 0,-1);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,-1);
   glEnd();

   //  Right
   glColor4fv(color);
   glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,color);
   glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,color);
   glNormal3f(+1, 0, 0);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,+1);
   glEnd();

   //  Left
   glColor4fv(color);
   glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,color);
   glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,color);
   glNormal3f(-1, 0, 0);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();

   //  Top
   glColor4fv(color);
   glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,color);
   glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,color);
   glNormal3f( 0,+1, 0);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,+1,+1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();

   //  Bottom
   glColor4fv(color);
   glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,color);
   glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,color);
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
      glBindTexture(GL_TEXTURE_2D,tex);
      glEnable(GL_TEXTURE_2D);
      SolidCube();
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
