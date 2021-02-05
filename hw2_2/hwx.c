/*
 *  NDC->RGB Shader
 *  Demonstrates use of vertex and fragment shaders
 *
 *  Key bindings:
 *  m          Toggle mode (shaders for this case)
 *  o          Change object
 *  arrows     Change view angle
 *  a/d        Rotate tower
 *  +/-        Change tower size
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include "CSCIx239.h"
int mode=1;    //  Shader
int obj=0;
int th=0,ph=0; //  View angles
int zh=0;      //  Light angle
int fov=55;    //  Field of view (for perspective)
int tex=0;     //  Texture
int shader[2]={0,0};  //  Shader
float asp=1;   //  Aspect ratio
float dim=3;   //  Size of world

// bounding box for objects, passed to shader
float bounding_box[3] = {1.0, 1.0, 1.0}; // "radius" of bounding box for shader
// material properties for rectangle
float rect_mat[2][4] = {{0,0,0,1},{0,0,0,1}};
// step for calculations
int step = 0;
//model location
int X=0, Y=0, Z=1;

// Store variable text here
const char* text[] = {"Fixed Pipeline",
                      "Silly Shader"};

//
//  Refresh display
//
void display(GLFWwindow* window)
{
  // clear window/depth buffer
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  // enable z-buffering
  glEnable(GL_DEPTH_TEST);
  // set view (uses CSCIx239 library)
  View(th, ph, fov, dim);

  //===SHADERS===
  // lighting
  Lighting(3*Cos(zh),1.5,3*Sin(zh) , 0.3,0.5,0.8);

  // enable shader by default
  glUseProgram(shader[mode]);
  // set uniforms
  int id = glGetUniformLocation(shader[mode],"loc");
  glUniform3f(id, X, Y, 1/Z);
  id = glGetUniformLocation(shader[mode], "bound");
  glUniform3f(id, bounding_box[0], bounding_box[1], bounding_box[2]);
  id = glGetUniformLocation(shader[mode],"stp");
  glUniform1i(id, step);
  id = glGetUniformLocation(shader[mode],"iterations");
  glUniform1i(id, step);
  //===DRAW===
  // set material properties
  glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,32.0);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,rect_mat[0]);
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,rect_mat[1]);
  /*
  // set normal vector
  glNormal3f( 0, 0, 1);
  glColor3f(1,1,1);
  // draw rect with tex coords
  float rx = rect_radius[0]; // x-coordinates
  float ry = rect_radius[1]; // y-coordinates
  glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex3f(-rx,-ry, 0);
    glTexCoord2f(1,0); glVertex3f(+rx,-ry, 0);
    glTexCoord2f(1,1); glVertex3f(+rx,+ry, 0);
    glTexCoord2f(0,1); glVertex3f(-rx,+ry, 0);
  glEnd();
  */
  switch(obj) {
    case 0 : {
      TexturedCube(tex);
      break;
    }
    case 1 : {
      TexturedIcosahedron(tex);
      break;
    }
    case 2 : {
      TexturedSphere(10, tex);
      break;
    }
    default: {
      obj = 0; //switch to cube if something bad happens
      break;
    }
  }

  // disable shader
  glUseProgram(0);
  //show axes
  Axes(2);
  // show properties
  glColor3f(1,1,1);
  glWindowPos2i(5,5);
  Print("Angle=%d,%d  Dim=%.1f Projection=%s Bound-Radius=%.1f Step=%d",th,ph,dim,fov>0?"Perpective":"Orthogonal",bounding_box[0],step);
  // check for errors (CSCIx239)
  ErrCheck("display");
  // flush (make sure GL actually does the stuff)
  glFlush();
  // swap display buffer (render the scene)
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
  else if (key==GLFW_KEY_A) {
    step--; step%=1000;
  }
  else if (key==GLFW_KEY_D) {
    step++; step%=1000;
  }
  else if (key==GLFW_KEY_EQUAL) {
    for(int n=0; n<3; n++) {
      bounding_box[n] += .1;
    }
  }
  else if (key==GLFW_KEY_MINUS) {
    for(int n=0; n<3; n++) {
      bounding_box[n] -= .1;
    }
  }
  // Reset view angle
  else if (key==GLFW_KEY_0)
    th = ph = 0;
  // Switch shaders
  else if (key==GLFW_KEY_M) {
    mode++; mode%=2;
  }
  //switch objects
  else if (key==GLFW_KEY_O) {
    obj++; obj%=3;
    printf("o");
  }
  // Switch between perspective/orthogonal
  else if (key==GLFW_KEY_P)
    fov = fov ? 0 : 57;
  // Increase/decrease asimuth
  else if (key==GLFW_KEY_RIGHT)
    th += 5;
  else if (key==GLFW_KEY_LEFT)
    th -= 5;
  // Increase/decrease elevation
  else if (key==GLFW_KEY_UP)
    ph += 5;
  else if (key==GLFW_KEY_DOWN)
    ph -= 5;
  // PageUp key - increase dim
  else if (key==GLFW_KEY_PAGE_DOWN)
    dim += 0.1;
  // PageDown key - decrease dim
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
}

//
//  Main program with GLFW event loop
//
int main(int argc,char* argv[])
{
   //  Initialize GLFW
   GLFWwindow* window = InitWindow("James Nichols: Homework X",1,600,600,&reshape,&key);

   //  Load shader
   shader[0] = CreateShaderProg("bound_map.vert","bound_test.frag");
   shader[1] = CreateShaderProg("model.vert","fractal.frag");
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
      zh++; zh%=360;
   }
   //  Shut down GLFW
   glfwDestroyWindow(window);
   glfwTerminate();
   return 0;
}
