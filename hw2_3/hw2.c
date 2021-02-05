/*
 *  NDC->RGB Shader
 *  Demonstrates use of vertex and fragment shaders
 *
 *  Key bindings:
 *  m          Toggle shader
 *  o          Change objects
 *  +/-        Increase/decrease iterations
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include "CSCIx239.h"
int mode=0;         //  Shader
int count=0;        //  Number of iterations
float zoom = 1.0;   //  Zoom into object
int th=0,ph=0;      //  View angles
int zh=80;          //  Light angle
int move_light = 0; // Toggle for light movement
int fov=55;         //  Field of view (for perspective)
int tex=0;          //  Texture
int obj=0;          //  Object
int place = 0;      //  Placement of object (see origin)
float origin[3][3] = {{0,0,0},{0,0,-1},{1,1,-1}}; //ratios for translation
int shader[3]={0,0,0}; //  Shaders
float asp=1;        //  Aspect ratio
float dim=3;        //  Size of world
char* text_obj[3]={"Cube", "Icosahedron", "Sphere"};
char* text_sha[4]={"None", "Simple", "Colored", "Checkerboard"};

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
   // Set up lighting
   // Doing this outside of transform so you always get light where you're viewing the fractal
   Lighting(3*Cos(zh), 1.5, 3*Sin(zh), .3, .5, .8);

   // apply transformations
   glPushMatrix();
   //move
   glTranslated(zoom*origin[place][0],zoom*origin[place][1],zoom*origin[place][2]);
   glScalef(zoom,zoom,zoom);
   // zoom all objects

   //  Enable shader
   if (mode)
   {
     glUseProgram(shader[mode-1]);
     int id = glGetUniformLocation(shader[mode-1],"iterations");
     glUniform1i(id,count);
   }
   else
     glUseProgram(0);
   //  Draw scene
   switch(obj) {
     case 0: {
       TexturedCube(tex);
       break;
     }
     case 1: {
       TexturedIcosahedron(tex);
       break;
     }
     case 2: {
       TexturedSphere(10, tex);
       break;
     }
   }
   //  Revert to fixed pipeline
   glUseProgram(0);
   // pop matrix
   glPopMatrix();

   //  Display axes
   Axes(2);
   //  Display parameters
   glColor3f(1,1,1);
   glWindowPos2i(5,20);
   Print("Angle=%d,%d  Zoom=%.1f Projection=%s Iterations=%d",th,ph,zoom,fov>0?"Perpective":"Orthogonal",count);
   glWindowPos2i(5,5);
   Print("Object=%s Position=%d Shader=%s", text_obj[obj], place, text_sha[mode]);
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
  //  Reset view angle
  else if (key==GLFW_KEY_0)
    th = ph = 0;
  //  Switch shaders
  else if (key==GLFW_KEY_M) {
    mode++;
    mode %= 4;
  }
  //  Switch objects
  else if (key==GLFW_KEY_O) {
    obj++; obj%=3;
  }
  //  Switch positions
  else if (key==GLFW_KEY_I) {
    place++; place%=3;
  }
  //  Toggle light movement
  else if (key==GLFW_KEY_L) {
    move_light = !move_light;
  }
  // increase/decrease fractal iterations
  else if (key==GLFW_KEY_EQUAL) {
    count++;
  }
  else if (key==GLFW_KEY_MINUS) {
    count--;
    if(count < 0) count = 0;
  }
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
  //  PageUp key - increase zoom
  else if (key==GLFW_KEY_PAGE_DOWN)
    zoom /= 1.2;
  //  PageDown key - decrease zoom
  else if (key==GLFW_KEY_PAGE_UP && dim>1)
    zoom *= 1.2;

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
   GLFWwindow* window = InitWindow("James Nichols: Homework 2",1,600,600,&reshape,&key);

   //  Load shader
   shader[0] = CreateShaderProg("model.vert","fractal_simple.frag");
   shader[1] = CreateShaderProg("model.vert","fractal_color.frag");
   shader[2] = CreateShaderProg("model.vert","fractal_check.frag");
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
      //  Increment lights. This probably shouldn't be done here. Too bad!
      if(move_light) {
        zh++; zh%=360;
      }
   }
   //  Shut down GLFW
   glfwDestroyWindow(window);
   glfwTerminate();
   return 0;
}
