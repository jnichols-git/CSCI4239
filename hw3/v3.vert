//  Confetti Cannon V3
// So I guess I have to try a bit harder to reduce performance. Fine.
// Removed if statement with step, but gonna throw in some arbitrary
// garbage to see what the compiler does about it.
#version 120

uniform   float time;  //  Time
attribute float start; //  Start time
attribute vec3  vel;   //  Velocity

void main(void)
{
  //  Particle location
  vec4 vert = gl_Vertex;
  //  Time offset mod 5
  float t = mod(time,5.0)-start;
  // 0 if t<0.0, 1 if t>=0.0
  float t_stp = step(0.0,t);

  vec4 col = vec4(0);
  for(int n=0; n<4096; n++) {
    col += gl_Color/4096.0;
  }

  //  Particle color
  gl_FrontColor = col*t_stp;
  //  Particle trajectory
  vert   += t*vec4(vel,0)*t_stp;
  //  Gravity
  vert.y -= 4.9*t*t*t_stp;
  //  Transform particle location
  gl_Position = gl_ModelViewProjectionMatrix*vert;
}
