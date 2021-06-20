/*
 *  Homework 7:
 *  Shows the use of stored textures to pass information to shaders
 */
#include "CSCIx239.h"
int mode=0;    // Control program behavior
int th=0,ph=0; // View angles
int fov=55;    // Field of view (for perspective)
int tex=0;     // Texture
int obj=0;     // Object
int shader[2]={0,0};  // Graphical shader
float asp=1;   // Aspect ratio
float dim=3;   // Size of world

#define MODE 2
#define OBJ  2
char* text[MODE] = {(char*)"Particles",(char*)"Ripples"};

// Plane
unsigned int plane_vbo = 0;
int plane_size = 6;
const float plane_data[] = {
  -1,0,-1,+1,   0,-1, 0,   1,0,1,1,  0,0,
  +1,0,-1,+1,   0,-1, 0,   1,0,1,1,  1,0,
  -1,0,+1,+1,   0,-1, 0,   1,0,1,1,  0,1,
  +1,0,-1,+1,   0,-1, 0,   1,0,1,1,  1,0,
  -1,0,+1,+1,   0,-1, 0,   1,0,1,1,  0,1,
  +1,0,+1,+1,   0,-1, 0,   1,0,1,1,  1,1
};


// Texture properties
const int   TEX_SIZE = 128;

//===PARTICLE FUNCTIONS (MODE 0)===

// Particle properties
const float PART_DENS = 0.05;
float rad = 1.0;
const char* rip_text[] = {"Single", "Raindrops"};

float randf(float min, float max) {
  float range = max-min;
  float val = (float)rand()/(float)(RAND_MAX/range);
  val += min;
  return val;
}

// Choose if a coordinate is valid for a particle
bool validCoord(int x, int y, int z, float density) {
  return randf(0.0,1.0) < density;
}

// Generate a 3D noise texture
// High density = more non-zero pixels
int genNoise(int tex_unit, float density) {

  float* pix = new float[TEX_SIZE*TEX_SIZE*TEX_SIZE*4]; // RGBA array of pixels
  float* ptr = pix;
  for(int z=0; z<TEX_SIZE; z++) {
    for(int y=0; y<TEX_SIZE; y++) {
      for(int x=0; x<TEX_SIZE; x++) {

        // Store r,g,b at random intensity
        if(validCoord(x,y,z,density)) {
          *ptr++ = randf(0.6,1.0); // R
          *ptr++ = randf(0.6,1.0); // G
          *ptr++ = randf(0.6,1.0); // B
          *ptr++ = randf(0.0,1.0); // Time point
        } else {
          for(int n=0; n<4; n++) {
            *ptr++ = 0.0;
          }
        }

      }
    }
  }

  // Set to GL texture
  glActiveTexture(tex_unit);
  unsigned int tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);

  // Copy noise
  glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, TEX_SIZE, TEX_SIZE, TEX_SIZE, 0, GL_RGBA, GL_FLOAT, pix);

  // Set params
  glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  // Set back to default texture
  glActiveTexture(GL_TEXTURE0);

  // Delete allocated memory
  delete[] pix;
  return tex;
}

int genTex(int tex_unit, const char* file) {
  glActiveTexture(tex_unit);
  int tex = LoadTexBMP(file);
  glActiveTexture(GL_TEXTURE0);
  return tex;
}

//===RIPPLE FUNCTIONS (MODE 1)===

// Ripple properties
int rmode = 0;
#define RMODE 2;
int RIP_CT      = 1;        // # of ripples
float RIP_SIZE  = 100;       // radial size of ripples
float P = 2.0;
float W = 0.6;
float M = 0.4;
// randf values
float min_ct = 0.3; float max_ct = 0.7; // cycle time
float min_rw = 0.5; float max_rw = 1.0; // ripple width
float min_rm = 0.8; float max_rm = 1.0; // ripple magnitude


/*
 * Distance from a,b to x,y
*/
float dist(float a, float b, float x, float y) {
  float dx = a-x;
  float dy = b-y;
  float dist = sqrt(pow(dx,2) + pow(dy,2));
  return dist;
}

/*
 * Generate a 3D texture to store ripple data.
 * x,y: contains data for 1 ripple
 *   z: ripple "index"
*/
int genRipples(int tex_unit) {
  float* pix = new float[TEX_SIZE*TEX_SIZE*RIP_CT*4]; // RGBA array of pixels
  float* ptr = pix;

  for(int z=0; z<RIP_CT; z++) {
    float cycle_time = randf(min_ct,max_ct); // Time in cycle that ripple starts (0->MAX_CYCLE)
    float rip_width  = randf(min_rw,max_rw);  // Ripple width (0->MAX_WIDTH)
    float rip_magnitude = randf(min_rm,max_rm);
    float px = rand()%(TEX_SIZE-40) + 20; // Center x
    float py = rand()%(TEX_SIZE-40) + 20; // Center y
    for(int y=0; y<TEX_SIZE; y++) {
      for(int x=0; x<TEX_SIZE; x++) {
        float r = cycle_time;                          // r = time in cycle
        float g = (RIP_SIZE-dist(x,y,px,py))/RIP_SIZE; // g = time of peak at pixel
        float b = rip_width;                           // b = ripple width
        float a = rip_magnitude;                                 // a = magnitude

        // If g>0, wave exists at some point
        if(g>0.0) {
          *ptr++ = r;
          *ptr++ = g;
          *ptr++ = b;
          *ptr++ = a;
        }
        // Otherwise just keep white
        else {
          for(int n=0; n<4; n++) {
            *ptr++ = 0.0;
          }
        }

      }
    }
  }
  // Set to GL texture
  glActiveTexture(tex_unit);
  unsigned int tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);

  // Copy noise
  glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, TEX_SIZE, TEX_SIZE, RIP_CT, 0, GL_RGBA, GL_FLOAT, pix);

  // Set params
  glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  // Set back to default texture
  glActiveTexture(GL_TEXTURE0);

  // Delete allocated memory
  delete[] pix;
  return tex;
}


// Initialize plane with predefined data
// Currently used in ripple mode
void initPlane() {
  glGenBuffers(1, &plane_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, plane_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(plane_data), plane_data, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}


// Meant to generate a (theoretically more efficient) 128x128 plane for ripples.
// Currently non-functional. Working on it.
void genPlaneVBO() {
  // Data array: 1x1 squares, 6 verts each, 13 floats per
  float* plane_dat = new float[1*1*6*13];
  plane_size   = 6;

  float px,py,pz,pw;
  float nx,ny,nz;
  float cr,cg,cb,ca;
  float ts,tt;
  float* p = plane_dat; // Array pointer

  nx = 0; ny = 1; nz = 0;
  cr = cg = cb = ca = 1.0;

  // V1
  px = -1; py = -1; pz = -1; pw = +1;
  ts = 0; tt = 0;
  *p++ = px; *p++ = py; *p++ = pz; *p++ = pw;
  *p++ = nx; *p++ = ny; *p++ = nz;
  *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
  *p++ = ts; *p++ = tt;
  // V2
  px = +1; py = -1; pz = -1; pw = +1;
  ts = 1; tt = 0;
  *p++ = px; *p++ = py; *p++ = pz; *p++ = pw;
  *p++ = nx; *p++ = ny; *p++ = nz;
  *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
  *p++ = ts; *p++ = tt;
  // V3
  px = -1; py = -1; pz = +1; pw = +1;
  ts = 0; tt = 1;
  *p++ = px; *p++ = py; *p++ = pz; *p++ = pw;
  *p++ = nx; *p++ = ny; *p++ = nz;
  *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
  *p++ = ts; *p++ = tt;
  // V4
  px = +1; py = -1; pz = +1; pw = +1;
  ts = 1; tt = 0;
  *p++ = px; *p++ = py; *p++ = pz; *p++ = pw;
  *p++ = nx; *p++ = ny; *p++ = nz;
  *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
  *p++ = ts; *p++ = tt;
  // V5
  px = -1; py = -1; pz = -1; pw = +1;
  ts = 0; tt = 1;
  *p++ = px; *p++ = py; *p++ = pz; *p++ = pw;
  *p++ = nx; *p++ = ny; *p++ = nz;
  *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
  *p++ = ts; *p++ = tt;
  // V6
  px = +1; py = -1; pz = +1; pw = +1;
  ts = 1; tt = 1;
  *p++ = px; *p++ = py; *p++ = pz; *p++ = pw;
  *p++ = nx; *p++ = ny; *p++ = nz;
  *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
  *p++ = ts; *p++ = tt;

  // Create VBO
  glGenBuffers(1, &plane_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, plane_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(plane_dat), plane_dat, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  delete[] plane_dat;
}


//===OpenGL FUNCTIONS===

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

  //  Enable shader
  glUseProgram(shader[mode]);
  // Set uniforms
  int id;
  id = glGetUniformLocation(shader[mode],"time");
  glUniform1f(id, glfwGetTime());

  glPushMatrix();
  //  Draw scene
  int count = sqrt(ceil(rad)) + 1;

  // Draw with v1
  if(mode == 0) {
    // Set view
    View(th,ph,fov,dim);
    // Set uniforms for particles
    id = glGetUniformLocation(shader[mode],"noise");
    glUniform1i(id,1); // Noise is generated to texture 1
    id = glGetUniformLocation(shader[mode],"tile");
    glUniform1f(id,1.0);
    // Set first cube size to rad
    float sub_radius = rad;
    glScalef(rad,rad,rad);
    // Draw count cubes, each half size of previous
    for(int n=0; n<count; n++) {
      // Draw cube with radius = sub_radius
      glUniform1f(id, sub_radius);
      SolidCube();
      // Scale next cube by 1/2
      glScalef(0.5, 0.5, 0.5);
      sub_radius /= 2;
    }
  } else
  // Draw with v2
  if(mode == 1) {
    // Bind plane buffer
    glBindBuffer(GL_ARRAY_BUFFER, plane_vbo);
    // Calculate eye position Ex,y,z
    float Ex = -2*dim*Sin(th)*Cos(ph);
    float Ey = +2*dim        *Sin(ph);
    float Ez = +2*dim*Cos(th)*Cos(ph);
    // Generate matrices
    float proj[16];
    mat4identity(proj);
    mat4perspective(proj, fov, asp, dim/16, 16*dim);
    float view[16];
    mat4identity(view);
    mat4lookAt(view, Ex,Ey,Ez, 0,0,0, 0,Cos(ph),0);
    float modelview[16];
    // Set uniforms for ripples
    id = glGetUniformLocation(shader[mode],"Projection"); glUniformMatrix4fv(id,1,0,proj);
    id = glGetUniformLocation(shader[mode], "ripple"); glUniform1i(id,2);
    id = glGetUniformLocation(shader[mode], "count"); glUniform1i(id, RIP_CT);
    id = glGetUniformLocation(shader[mode], "P"); glUniform1f(id, P);
    id = glGetUniformLocation(shader[mode], "W"); glUniform1f(id, W);
    id = glGetUniformLocation(shader[mode], "M"); glUniform1f(id, M);
    // Set/enable attributes

    glVertexAttribPointer(0,4,GL_FLOAT,0,52,(void*) 0); // Vertex
    glVertexAttribPointer(1,2,GL_FLOAT,0,52,(void*)44); // Texture
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Draw plane
    // The below is what happens when I do work at midnight.
    for(int z=-64; z<64; z++) {
      for(int x=-64; x<64; x++) {
        float px = (float)(x+64); float py = -1; float pz = (float)(z+64);
        px /= 128; pz /= 128;
        id = glGetUniformLocation(shader[mode], "pos");
        glUniform3f(id, px, py, pz);
        // Calculate ModelView
        mat4copy(modelview, view);
        mat4scale(modelview, 3.0/128.0, 1.0, 3.0/128.0);
        mat4translate(modelview, (float)x*2, -1, (float)z*2);
        id = glGetUniformLocation(shader[mode], "ModelView");
        glUniformMatrix4fv(id, 1, 0, modelview);
        glDrawArrays(GL_TRIANGLES, 0, plane_size);
      }
    }

    // Disable attributes
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    // Unbind plane buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  glPopMatrix();


  //  Revert to fixed pipeline
  glUseProgram(0);

  //  Display axes
  Axes(2);
  //  Display parameters
  glColor3f(1,1,1);
  glWindowPos2i(5,5);
  if(mode==0) {
    Print("Angle=%d,%d Mode=%s, Volume Size=%f",th,ph,text[mode],rad);
  } else {
    Print("Angle=%d,%d Mode=%s, Ripple Type=%s",th,ph,text[mode],rip_text[rmode]);
  }
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
    // Generate texture depending on mode

    if(mode==0) genNoise(GL_TEXTURE1, PART_DENS);
    else if(mode==1) genRipples(GL_TEXTURE2);

  }
  //  Switch objects
  else if (key==GLFW_KEY_O) {
    obj++;
    obj %= OBJ;
  }
  // Increase/decrease volume radius
  else if (key==GLFW_KEY_MINUS) {
    rad -= 0.1;
  }
  else if (key==GLFW_KEY_EQUAL) {
    rad += 0.1;
  }
  else if (key==GLFW_KEY_R) {
    rmode++; rmode%=RMODE;
    if(rmode==0) {
      RIP_CT = 1;
      RIP_SIZE = 100;
      P = 2.0;
      W = 0.6;
      M = 0.4;
    } else
    if(rmode==1) {
      RIP_CT = 32;
      RIP_SIZE = 15;
      P = 0.5;
      W = 0.2;
      M = 0.05;
    }
    genRipples(GL_TEXTURE2);
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
  // Seed rng
  srand(time(NULL));
  //  Initialize GLFW
  GLFWwindow* window = InitWindow("Homework 1: James Nichols",1,600,600,&reshape,&key);

  // Generate first noise texture
  genNoise(GL_TEXTURE1, PART_DENS);
  // Load particle shader
  shader[0] = CreateShaderProg("part.vert","part.frag");
  // Load ripple shader
  shader[1] = CreateShaderProgGeom("rip.vert","rip.geom","rip.frag");
  // Load ripple surface
  initPlane();
  //genPlaneVBO();

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
