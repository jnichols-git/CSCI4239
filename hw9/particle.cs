//
//  Particle compute shader
//
#version 440 compatibility
#extension GL_ARB_compute_variable_group_size : enable

//  Array positions
layout(binding=4) buffer posbuf {vec4 pos[];};
layout(binding=5) buffer delbuf {float del[];};
layout(binding=6) buffer colbuf {vec4 col[];};
//  Work group size
layout(local_size_variable) in;

// Volume
uniform float density;
// Time
uniform float time;
// Random
uniform vec3 rand_seed;
// Timestep
const float  dt = 0.1;

float rand(vec3 n) {
  return fract(sin(dot(n, rand_seed))*43758.5453);
}

// Calculate particle strength (alpha) based on 10 second loop
float calcStrength() {
  // Time in loop
  float loop = mod(time,10);
  // Delat time
  float delay = del[gl_GlobalInvocationID.x];
  float s = 0;
  // gotta get that smooth transition
  for(int n=-1; n<=1; n++) {
    float offset = loop+(n*10);
    if(abs(offset-delay) <= 1.0) s = 1.0-abs(offset-delay);
  }
  return s;
}

//  Compute shader
void main()
{
   //  Global Thread ID
   uint  gid = gl_GlobalInvocationID.x;

   //  Get position and velocity
   vec3 p0 = pos[gid].xyz;

   //  Compute new position and velocity
   vec3 p = p0;//p0 + v0*dt + 0.5*dt*dt*G;
   float a = rand(p0*2);
   a = step(a,density) * calcStrength();


   // Do RNG

   //  Update position and velocity
   pos[gid].xyz  = p;
   //col[gid].xyz = vec3(density);
   col[gid].a = a;
}
