/*
 *  hwx
 *  [Description]
 *
 *  Key bindings:
 *  m          Toggle shader
 *  o          Change objects
 *  arrows     Change view angle
 *  a/d        Rotate brick "tower"
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include "CSCIx239.h"
int mode=0;    //  Shader
int th=0,ph=0; //  View angles
int zh=0; // lighting angle
float rot=0; // rotation of brick "tower"
int X=0, Y=0, Z=1; // position of object
int fov=55;    //  Field of view (for perspective)
int tex=0;     //  Texture
int obj=0;     //  Object
int shader=0;  //  Shader
float asp=1;   //  Aspect ratio
float dim=3;   //  Size ofid = glGetUniformLocation(shader[mode],"loc");
float mat_prop[2][4] = {{0,0,0,1},{0,0,0,1}};
const char* text[] = {"Fixed Pipeline","Silly Shader"};

//
//  Refresh display
//
void display(GLFWwindow* window)
{
  //  Erase the window and the depth buffer
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  //  Enable Z-buffering in OpenGL
  glEnable(GL_DEPTH_TEST);
  //  Set view
  View(th,ph,fov,dim);

  // Enable lighting
  Lighting(3*Cos(zh),1.5,3*Sin(zh) , 0.3,0.5,0.8);
  //  Enable shader
  if (mode)
  {
    glUseProgram(shader);
    int id = glGetUniformLocation(shader,"loc");
    glUniform3f(id,X,Y,1/Z);
    id = glGetUniformLocation(shader,"rotation");
    glUniform1f(id, rot);
    id = glGetUniformLocation(shader,"obj_size");
    glUniform3f(id,4,20,0);
  }
  else
    glUseProgram(0);
  //  Draw scene
  if (obj)
    TexturedIcosahedron(tex);
  else {
    //TexturedCube(tex);
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,32.0);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,mat_prop[0]);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,mat_prop[1]);
    glNormal3f( 0, 0, 1);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex3f(-2,-10, 0);
    glTexCoord2f(1,0); glVertex3f(+2,-10, 0);
    glTexCoord2f(1,1); glVertex3f(+2,+10, 0);
    glTexCoord2f(0,1); glVertex3f(-2,+10, 0);
    glEnd();
  }
  //  Revert to fixed pipeline
  glUseProgram(0);

  //  Display axes
  Axes(2);
  //  Display parameters
  glColor3f(1,1,1);
  glWindowPos2i(5,5);
  Print("Angle=%d,%d  Dim=%.1f Projection=%s Mode=%s",th,ph,dim,fov>0?"Perpective":"Orthogonal",text[mode]);
  //  Render the scene and make it visible
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

  //  Exit on ESC
  if (key == GLFW_KEY_ESCAPE)
    glfwSetWindowShouldClose(window,1);
  // Move tower
  else if(key==GLFW_KEY_A) {
    rot++;
    if(rot==-360) rot = 0;
  }
  else if(key==GLFW_KEY_D) {
    rot--;
    if(rot==360) rot = 0;
  }
  //  Reset view angle
  else if (key==GLFW_KEY_0)
    th = ph = 0;
  //  Switch shaders
  else if (key==GLFW_KEY_M)
    mode = 1-mode;
  //  Switch objects
  else if (key==GLFW_KEY_O)
    obj = 1-obj;
  //  Switch between perspective/orthogonal
  else if (key==GLFW_KEY_P)
    fov = fov ? 0 : 57;
  //  Increase/decrease asimuth
  else if (key==GLFW_KEY_RIGHT)
    th += 5;
  else if (key==GLFW_KEY_LEFT)
    th -= 5;
  //  Increase/decrease elevation
  else if (key==GLFW_KEY_UP)
    ph += 5;
  else if (key==GLFW_KEY_DOWN)
    ph -= 5;
  //  PageUp key - increase dim
  else if (key==GLFW_KEY_PAGE_DOWN)
    dim += 0.1;
  //  PageDown key - decrease dim
  else if (key==GLFW_KEY_PAGE_UP && dim>1)
    dim -= 0.1;

   //  Wrap angles
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
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Projection(fov,asp,dim);
   // Make sure shaders get correct screen size
   int id = glGetUniformLocation(shader,"window");
   glUniform2f(id,width,height);
}

//
//  Main program with GLFW event loop
//
int main(int argc,char* argv[])
{
   //  Initialize GLFW
   GLFWwindow* window = InitWindow("James Nichols: Homework X",1,600,600,&reshape,&key);

   //  Load shader
   shader = CreateShaderProg("model_mapped.vert","brick_mapped.frag");
   //  Load textures
   tex = LoadTexBMP("pi.bmp");

   //  Event loop
   ErrCheck("init");
   while(!glfwWindowShouldClose(window))
   {
      //  Display
      display(window);
      //  Process any events
      glfwPollEvents();
      // increment light
      zh++;
   }
   //  Shut down GLFW
   glfwDestroyWindow(window);
   glfwTerminate();
   return 0;
}
