// All code that uses the x239 library will be commented as such.
#include "CSCIx239.h"

// Constants]
const int MODE_CT   = 2;
const int SHADER_CT = 4;      // shader count
const int TEX_CT    = 1;      // texture count
const int OBJ_CT    = 2;      // object count

// Definitions
#define VELOCITY_ARRAY 4
#define START_ARRAY 5
char* Name[] = {(char*)"",(char*)"",(char*)"",(char*)"",(char*)"vel",(char*)"start",NULL};

// Variables
int   shader[SHADER_CT]={};   // identifier for shader program
int   texture[TEX_CT]={};     // identifiers for loaded textures
void  (*object[OBJ_CT])(int); // store function pointers for objects. probably dumb. Too bad!
int   th=0, ph=0;             // camera view angles
int   fov=55;                 // field of view
float dim=3;                  // size of world
int   mode=0;                 // controls general program stuff
int   mode_sh=0;              // controls which shader to use
int   mode_obj=0;             // controls which object to use
char* text[MODE_CT]={(char*)"CPU", (char*)"Shader"};
char* text_sh[SHADER_CT]={(char*)"V1", (char*)"V2", (char*)"V3", (char*)"V4"};

// Particle variables
float volume_pos[3] = {0.0, 0.0, 0.0};
float volume_rad    = .3;
int   p_ct = 10000;
float* p_pos;
float* p_vel;
float* p_col;
float* p_start;
float  last_start;      // Last start time of particle sim
float  last_step;       // Last step taken by particle sim, unlock particles from fps during CPU calc
int    last_p_ct = 10000; // Previous count of rendered particles

/*
 * Function to initialize important variables
 * I like having this at the top for convenience.
*/
void setup() {
  // Create shaders (x239)
  shader[0] = CreateShaderProgAttr((char*)"v1.vert", NULL, Name);
  shader[1] = CreateShaderProgAttr((char*)"v2.vert", NULL, Name);
  shader[2] = CreateShaderProgAttr((char*)"v3.vert", NULL, Name);
  shader[3] = CreateShaderProgAttr((char*)"v4.vert", NULL, Name);
  // Load textures (x239)
  texture[0] = LoadTexBMP("pi.bmp");
  // Store object functions
  object[0] = TexturedCube;
  object[1] = TexturedIcosahedron;
}

/*
 * Random float from ex16
*/
static float frand(float rng,float off) {
   return rand()*rng/RAND_MAX+off;
}

/*
 * Particle generation, derived from ex16
*/
void generateParticles() {
  srand(rand());
  // Array pointers
  delete[] p_pos; p_pos = new float[3*p_ct]; float* pos = p_pos;
  delete[] p_vel; p_vel = new float[3*p_ct]; float* vel = p_vel;
  delete[] p_col; p_col = new float[3*p_ct]; float* col = p_col;
  delete[] p_start; p_start = new float[p_ct]; float* start = p_start;

  //determine min-max positions in particle volume
  for(int n=0; n<p_ct; n++) {
    *pos++ = frand(2,-1);
    *pos++ = frand(2,-1);
    *pos++ = frand(2,-1);
    *vel++ = frand(1.0,3.0);
    *vel++ = frand(10.0,0.0);
    *vel++ = frand(1.0,3.0);
    *col++ = frand(1.0,0.0);
    *col++ = frand(1.0,0.0);
    *col++ = frand(1.0,0.0);
    *start++ = frand(2.0,0.0);
  }
  last_start = glfwGetTime();
  last_step =  0.0; // reset step timer
  last_p_ct = p_ct;
}

/*
 * Generates particles with int values instead of floats
*/

/*
 * Sets particle position
 * Mostly derived from ex16's confetti.vert.
 * Horrible inefficient and kind of ugly, but I was curious how big a difference it would make to use CPU.
*/
void setPartPos() {
  float* pos = p_pos;
  float* vel = p_vel;
  //float* col = p_col;
  float* start = p_start;

  float time = glfwGetTime()-last_start;
  for(int n=0; n<last_p_ct; n++) {
    float t = time - *start++;
    //printf("%f\n", t);
    // set to black before start time
    if(t<0.0) {
      //*col++ = 0; *col++ = 0; *col++ = 0;
    } else {
      // calculate delta time
      float delta = time-last_step;
      // calculate new position
      *pos++ += *vel++ * delta; // move x
      *vel   += -9.81  * delta; // adjust for gravity
      *pos++ += *vel++ * delta; // move y
      *pos++ += *vel++ * delta; // move z
    }
  }
  last_step = time;
}

/*
 * Particle drawing from ex16
*/
void DrawPart(void) {
  //  Set particle size
  glPointSize(2);
  //  Point vertex location to local array Vert
  glVertexPointer(3,GL_FLOAT,0,p_pos);
  //  Point color array to local array Color
  glColorPointer(3,GL_FLOAT,0,p_col);
  //  Point attribute arrays to local arrays
  glVertexAttribPointer(VELOCITY_ARRAY,3,GL_FLOAT,GL_FALSE,0,p_vel);
  glVertexAttribPointer(START_ARRAY,1,GL_FLOAT,GL_FALSE,0,p_start);
  //  Enable arrays used by DrawArrays
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glEnableVertexAttribArray(VELOCITY_ARRAY);
  glEnableVertexAttribArray(START_ARRAY);
  //  Set transparent large particles
  // apply transformations based on volume
  glPushMatrix();
  glScaled(volume_rad,volume_rad,volume_rad);
  //  Draw arrays
  glDrawArrays(GL_POINTS,0,last_p_ct);
  glPopMatrix();
  //  Disable arrays
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableVertexAttribArray(VELOCITY_ARRAY);
  glDisableVertexAttribArray(START_ARRAY);
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
  if(mode) {
    glUseProgram(shader[mode_sh]);
    // Set shader uniforms
    int id;
    id = glGetUniformLocation(shader[mode_sh], "time");
    glUniform1f(id,glfwGetTime());
    id = glGetUniformLocation(shader[mode_sh], "arbi");
    glUniform1f(id, 4096.0);
  }
  else {
    glUseProgram(0);
    // set particle position on CPU
    setPartPos();
  }

  DrawPart();

  // Shaders off
  glUseProgram(0);

  // Display axes/text (x239)
  Axes(2);
  glColor3f(1,1,1);
  glWindowPos2i(5,20);
  Print("FPS: %d", FramesPerSecond());
  glWindowPos2i(5,5);
  Print("Method: %s ", text[mode]);
  if(mode > 0) {
    Print("%s ", text_sh[mode_sh]);
  }
  Print("Particles: %d", p_ct);
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
  // scale volume
  else if(key==GLFW_KEY_W) volume_rad += .05;
  else if(key==GLFW_KEY_S) volume_rad -= .05;
  // switch modes
  else if (key==GLFW_KEY_M) {
    mode++; mode%=MODE_CT;
    generateParticles();
  }
  // switch shaders
  else if (key==GLFW_KEY_P) {
    mode_sh++; mode_sh%=SHADER_CT;
    generateParticles();
  }
  // increase/decrease particle count (does not refresh!)
  else if (key==GLFW_KEY_EQUAL) p_ct *= 10;
  else if (key==GLFW_KEY_MINUS) p_ct /= 10;
  //keep minimum at 1
  if(p_ct==0) p_ct = 1;
  // regenerate particles
  else if (key==GLFW_KEY_R) generateParticles();
}

int main(int argc, char* argv[]) {
  // Start GLFW
  GLFWwindow* window = InitWindow("James Nichols: Homework X",0,600,600,&reshape, &key);
  // Setup variables
  setup();
  // Generate first particles
  generateParticles();
  // Start event loop
  while(!glfwWindowShouldClose(window)) {
    display(window);
    glfwPollEvents();
  }
  // Shut down GLFW
  glfwDestroyWindow(window);
  glfwTerminate();
  // Free up memory
  delete[] p_pos;
  delete[] p_vel;
  delete[] p_start;
  delete[] p_col;
  return 0;
}