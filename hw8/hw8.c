/*
 *  Homework 8:
 *  Uses a vertex shader to calculate particles
 *  interacting with a simple environment
 *
 *  Key bindings:
 *  m          Toggle shader
 *  o          Change objects
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include "CSCIx239.h"
int mode=0;    //  Shader
int th=0,ph=0; //  View angles
int fov=55;    //  Field of view (for perspective)
int obj=0;     //  Object
int shader[2] = {0,0};  //  Shader
float asp=1;   //  Aspect ratio
float dim=5;   //  Size of world

#define MODE 1
#define OBJ  2
char* text[MODE] = {"Particles"};

// Attributes
#define VELOCITY_ARRAY 4
#define START_ARRAY 5
static char* Name[] = {"","","","","VEL","STR",NULL};
// Particles
#define CT 200
float cycle = 10.0; // Time for one particle cycle to complete
float POS [3*CT*CT];
float COL [3*CT*CT];
float VEL [3*CT*CT];
float STR [CT*CT];
float origin[3] = {0,8,0};
// Plane
float plane_angle = 45;

static float frand(float min, float max) {
  float delta = max-min;
  return ((float)rand()/RAND_MAX)*delta + min;
}

void initParticles() {
  float* P = POS;
  float* C = COL;
  float* V = VEL;
  float* S = STR;
  for(int b=0; b<CT; b++) {
    for(int a=0; a<CT; a++) {
      // Position
      *P++ = frand(-1,1) + origin[0];
      *P++ = frand(-.5,.5) + origin[1];
      *P++ = frand(-1,1) + origin[2];
      // Color
      float b = frand(0.7,1.0);
      *C++ = .1;
      *C++ = frand(.3,.5);
      *C++ = b;
      // Velocity
      *V++ = frand(-.2,.2);
      *V++ = frand(-0.5,0);
      *V++ = frand(-.2,.2);
      // Start Time
      *S++ = frand(0.0,0.8);
    }
  }
}

void drawParticles() {
  glPointSize(4);
  // Set attributes
  glVertexPointer(3,GL_FLOAT,0,POS);
  glColorPointer(3,GL_FLOAT,0,COL);
  glVertexAttribPointer(VELOCITY_ARRAY,3,GL_FLOAT,GL_FALSE,0,VEL);
  glVertexAttribPointer(START_ARRAY,1,GL_FLOAT,GL_FALSE,0,STR);
  // Enable arrays
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glEnableVertexAttribArray(VELOCITY_ARRAY);
  glEnableVertexAttribArray(START_ARRAY);
  // Draw
  glDrawArrays(GL_POINTS,0,CT*CT);
  // Disable arrays
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableVertexAttribArray(VELOCITY_ARRAY);
  glDisableVertexAttribArray(START_ARRAY);
}

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
  // Draw particles
  glUseProgram(shader[1]);
  int id = glGetUniformLocation(shader[1],"time");
  glUniform1f(id,glfwGetTime());
  id = glGetUniformLocation(shader[1],"cycle");
  glUniform1f(id, cycle);
  id = glGetUniformLocation(shader[1],"angle");
  glUniform1f(id, plane_angle);
  //  Draw scene
  drawParticles();
  //  Revert to fixed pipeline
  glUseProgram(0);

  // Draw plane
  glPushMatrix();
  glRotatef(plane_angle, 0,0,1);
  glBegin(GL_QUADS);
  glVertex3f(-1,0,1);
  glVertex3f(1,0,1);
  glVertex3f(1,0,-1);
  glVertex3f(-1,0,-1);
  glEnd();
  glPopMatrix();

  //  Display axes
  Axes(2);
  //  Display parameters
  glColor3f(1,1,1);
  glWindowPos2i(5,5);
  Print("Angle=%d,%d Mode=%s",th,ph,text[mode]);
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
    mode %= MODE;
  }
  //  Switch objects
  else if (key==GLFW_KEY_O) {
    obj++;
    obj %= OBJ;
  }
  //  Increase/decrease asimu   shader[1] = CreateShadeth
  else if (key==GLFW_KEY_RIGHT)
    th += 5;
  else if (key==GLFW_KEY_LEFT)
    th -= 5;
  //  Increase/decrease elevation
  else if (key==GLFW_KEY_UP)
    ph += 5;
  else if (key==GLFW_KEY_DOWN)
    ph -= 5;
  // Rotate plane
  else if (key==GLFW_KEY_EQUAL) {
    plane_angle++;
    if(plane_angle >= 90) plane_angle = 90;
  }
  else if (key==GLFW_KEY_MINUS) {
    plane_angle--;
    if(plane_angle <= 0) plane_angle = 0;
  }

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
   GLFWwindow* window = InitWindow("Homework 8: James Nichols",1,600,600,&reshape,&key);

   //  Load shader
   shader[1] = CreateShaderProgAttr("part.vert",NULL, Name);
   // Initialize particles
   initParticles();

   //  Event loop
   ErrCheck("init");
   while(!glfwWindowShouldClose(window))
   {
      //  Display
      display(window);
      //  Process any events
      glfwPollEvents();
   }
   //  Shut down GLFW
   glfwDestroyWindow(window);
   glfwTerminate();
   return 0;
}
