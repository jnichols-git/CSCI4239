// All code that uses the x239 library will be commented as such.
#include "CSCIx239.h"

// Constants
const int SHADER_CT = 1;      // shader count
const int TEX_CT    = 1;      // texture count
const int OBJ_CT    = 2;      // object count

// Variables
int   shader[SHADER_CT]={};   // identifier for shader program
int   texture[TEX_CT]={};     // identifiers for loaded textures
void  (*object[OBJ_CT])(int); // store function pointers for objects. probably dumb. Too bad!
int   th=0, ph=0;             // camera view angles
int   fov=55;                 // field of view
float dim=3;                  // size of world
int   mode_sh=0;              // controls which shader to use
int   mode_obj=0;             // controls which object to use

/*
 * Function to initialize important variables
 * I like having this at the top for convenience.
*/
void setup() {
  // Create shaders (x239)
  //shader[0] = CreateShaderProg();
  // Load textures (x239)
  texture[0] = LoadTexBMP("pi.bmp");
  // Store object functions
  object[0] = TexturedCube;
  object[1] = TexturedIcosahedron;
}

/*
 * Display callback
*/
void display(GLFWwindow* window) {
  // Clear buffers, enable z-buffer
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  // Set view (x239)
  View(th, ph, fov, dim);

  // Switch between shaders
  if(mode_sh) {
    glUseProgram(shader[mode_sh-1]);
    // Set shader uniforms
  }
  else {
    glUseProgram(0);
  }

  // Draw scene here
  object[mode_obj](texture[0]);

  // Shaders off
  glUseProgram(0);

  // Display axes/text (x239)
  Axes(2);
  glColor3f(1,1,1);
  glWindowPos2i(5,5);

  // Render
  ErrCheck("display");
  glFlush();
  glfwSwapBuffers(window);
}

/*
 * Reshape callback
*/
void reshape(GLFWwindow* window, int width, int height) {
  glfwGetFramebufferSize(window,&width,&height);
  // aspect ratio
  float asp = (height>0) ? (double)width/height : 1;
  // Set viewport to window
  glViewport(0,0, width,height);
  // Set projection (x239)
  Projection(fov, asp, dim);
}

/*
 * Key callback
*/
void key(GLFWwindow* window, int key, int scancode, int action, int mods) {
  //  Discard key releases (keeps PRESS and REPEAT)
  if (action==GLFW_RELEASE) return;
  // Exit on ESC
  if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window,1);
  // Increase/decrease rotation
  else if (key==GLFW_KEY_RIGHT) th += 5;
  else if (key==GLFW_KEY_LEFT)  th -= 5;
  // Increase/decrease elevation
  else if (key==GLFW_KEY_UP)    ph += 5;
  else if (key==GLFW_KEY_DOWN)  ph -= 5;
  // switch objects
  else if (key==GLFW_KEY_O) {
    mode_obj++; mode_obj%=OBJ_CT;
  }
  // switch shaders
  else if (key==GLFW_KEY_P) {
    mode_sh++; mode_sh%=SHADER_CT+1;
  }
}

int main(int argc, char* argv[]) {
  // Start GLFW
  GLFWwindow* window = InitWindow("James Nichols: Homework X",1,600,600,&reshape, &key);
  // Setup variables
  setup();
  // Start event loop
  while(!glfwWindowShouldClose(window)) {
    display(window);
    glfwPollEvents();
  }
  // Shut down GLFW
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}