/*
 *  Homework X:
 *  Demonstrates use of vertex and fragment shaders
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
int tex=0;     //  Texture
int obj=0;     //  Object
int shader=0;  //  Shader
float asp=1;   //  Aspect ratio
float dim=4;   //  Size of world

// Shaders
GLint p_cshader;  // Particle compute
GLint p_shader;   // Particles
GLint env_shader; // Environment
// Buffers
GLuint POS;
GLuint DEL;
GLuint COL;
// Work groups
GLint wg_count;
GLint wg_size;
// Particles
int part_ct;
GLuint part_sprite;
float vol_size = 1.0;
float vol_ct   = 50; // particles in volume
// RNG
float r1, r2, r3;

// vec4 struct from ex19
typedef struct {
  union {float x;float r;};
  union {float y;float g;};
  union {float z;float b;};
  union {float w;float a;};
} vec4;

#define MODE 2
#define OBJ  2
char* text[MODE] = {(char*)"Mode 0",(char*)"Mode 1"};

static float randf(float min, float max) {
  return ((float)rand()/RAND_MAX)*(max-min)+min;
}

static int CreateShaderProgCompute(const char* file) {
   //  Create program
   int prog = glCreateProgram();
   //  Create and compile compute shader
   CreateShader(prog,GL_COMPUTE_SHADER,file);
   //  Link program
   glLinkProgram(prog);
   //  Check for errors
   PrintProgramLog(prog);
   //  Return name
   return prog;
}

static void generateParticles() {
  vec4  *pos,*col;
  float *del;
  GLenum buf = GL_SHADER_STORAGE_BUFFER;
  GLbitfield flags = GL_MAP_WRITE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT;
  // General loop; bind shader storage with buffers, populate, unmap
  // Position

  glBindBuffer(buf,POS);
  pos = (vec4*)glMapBufferRange(buf,0,part_ct*sizeof(vec4),flags);
  /*
  for(int n=0; n<part_ct; n++) {
    pos[n].x = randf(-100,100);
    pos[n].y = randf(400,600);
    pos[n].z = randf(-100,100);
    pos[n].w = 1;
  }
  */
  float incr = 1.0/100.0;
  for(int z=-50; z<50; z++) {
    for(int y=-50; y<50; y++) {
      for(int x=-50; x<50; x++) {
        int idx = (x+50) + (y+50)*100 + (z+50)*100*100;
        pos[idx].x = (float)x*incr*2;
        pos[idx].y = (float)y*incr*2;
        pos[idx].z = (float)z*incr*2;
        pos[idx].w = 1.0;
      }
    }
  }
  glUnmapBuffer(buf);
  // Delay
  glBindBuffer(buf,DEL);
  del = (float*)glMapBufferRange(buf,0,part_ct*sizeof(float),flags);
  for(int n=0; n<part_ct; n++) {
    del[n] = randf(0.0,10.0);
  }
  glUnmapBuffer(buf);
  // Color
  glBindBuffer(buf,COL);
  col = (vec4*)glMapBufferRange(buf,0,part_ct*sizeof(vec4),flags);
  for(int n=0; n<part_ct; n++) {
    col[n].r = randf(.1,1);
    col[n].g = randf(.1,1);
    col[n].b = randf(.1,1);
    col[n].a = 1;
  }
  glUnmapBuffer(buf);


  // Set buffer base
  glBindBufferBase(buf, 4, POS);
  glBindBufferBase(buf, 5, DEL);
  glBindBufferBase(buf, 6, COL);

  glBindBuffer(buf, 0);

  // Regenerate random seed
  r1 = randf(0,20); r2 = randf(0,20); r3 = randf(0,20);
  ErrCheck("generate");
}

static void initalizeBuffers() {
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT,0,&wg_count);
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0,&wg_size);
  if(wg_count > 8192) wg_count = 8192;
  // Trying this
  wg_count = 100*100;
  wg_size = 100;
  part_ct = wg_count*wg_size;
  // Shorthand a couple of things
  GLenum BUF = GL_SHADER_STORAGE_BUFFER;
  GLenum DRAW = GL_STATIC_DRAW;
  // Generate buffers
  // Position
  glGenBuffers(1,&POS);
  glBindBuffer(BUF,POS);
  printf("%d\n", part_ct);
  glBufferData(BUF,part_ct*sizeof(vec4),NULL,DRAW);
  ErrCheck("pos");
  // Velocity
  glGenBuffers(1,&DEL);
  glBindBuffer(BUF,DEL);
  glBufferData(BUF,part_ct*sizeof(float),NULL,DRAW);
  // Color
  glGenBuffers(1,&COL);
  glBindBuffer(BUF,COL);
  glBufferData(BUF,part_ct*sizeof(vec4),NULL,DRAW);
  glBindBuffer(BUF,0);
  // Call method to populate buffers
  ErrCheck("init");
  generateParticles();
}

// Draw particles
void drawParticles() {
  glPointSize(10);
  // Vertex array
  glBindBuffer(GL_ARRAY_BUFFER,POS);
  glVertexPointer(4,GL_FLOAT,0,(void*)0);
  // Color array
  glBindBuffer(GL_ARRAY_BUFFER,COL);
  glColorPointer(4,GL_FLOAT,0,(void*)0);
  // Enable arrays used by DrawArrays
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  // Enable transparency
  glEnable(GL_POINT_SPRITE);
  glTexEnvi(GL_POINT_SPRITE,GL_COORD_REPLACE,GL_TRUE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glDepthMask(0);
  // Draw arrays
  glDrawArrays(GL_POINTS,0,part_ct);
  // Reset transparency
  glDisable(GL_POINT_SPRITE);
  glDisable(GL_BLEND);
  glDepthMask(1);
  // Disable arrays
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  // Reset buffer
  glBindBuffer(GL_ARRAY_BUFFER,0);
}

static float calcDensityPct() {
  return (float)vol_ct/part_ct;
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

  /*
  //  Draw sphere
  glColor3f(0.8,0.8,0);
  glPushMatrix();
  glTranslatef(0,SY,0);
  glRotatef(-90,1,0,0);
  glScaled(SR,SR,SR);
  SolidSphere(32);
  glPopMatrix();
  ErrCheck("sphere");
  */

  // Zero out squares counter
  glUseProgram(p_cshader);
  int id = glGetUniformLocation(p_cshader,"density");
  glUniform1f(id,calcDensityPct());
  id = glGetUniformLocation(p_cshader,"time");
  glUniform1f(id,glfwGetTime());
  id = glGetUniformLocation(p_cshader,"rand_seed");
  glUniform3f(id, r1,r2,r3);
  glDispatchComputeGroupSizeARB(part_ct/wg_size,1,1,wg_size,1,1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  ErrCheck("compute");
  glUseProgram(p_shader);
  glPushMatrix();
  glScalef(vol_size,vol_size,vol_size);
  drawParticles();
  glPopMatrix();
  glUseProgram(0);
  ErrCheck("particles");


  //  Display axes
  Axes(2);
  //  Display parameters
  glColor3f(1,1,1);
  glWindowPos2i(5,20);
  Print("Angle=%d,%d Mode=%s FPS=%d",th,ph,text[mode],FramesPerSecond());
  glWindowPos2i(5,5);
  Print("Volume Size=%f Particle Count=%.0f/%d",vol_size,vol_ct,part_ct);
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
  else if (key==GLFW_KEY_R) {
    generateParticles();
  }
  // Increase/decrease volume size
  else if (key==GLFW_KEY_MINUS) {
    vol_size /= 1.2;
  }
  else if (key==GLFW_KEY_EQUAL) {
    vol_size *= 1.2;
  }
  // Increase/decrease particle count
  else if (key==GLFW_KEY_A) {
    float incr = 1.0;
    while(incr*10 < vol_ct) incr *= 10;
    vol_ct -= incr;
  }
  else if (key==GLFW_KEY_D) {
    float incr = 1.0;
    while(incr*10 <= vol_ct) incr *= 10;
    vol_ct += incr;
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
   GLFWwindow* window = InitWindow("Homework 1: James Nichols",0,600,600,&reshape,&key);

   //  Load shader
   p_cshader = CreateShaderProgCompute("particle.cs");
   p_shader  = CreateShaderProg("particle.vert","particle.frag");
   initalizeBuffers();
   glActiveTexture(GL_TEXTURE0);
   LoadTexBMP("particle_2.bmp");

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
