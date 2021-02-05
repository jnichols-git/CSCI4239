//  Confetti Cannon V3
// But what if I made it even *more* arbitrarily bad?

#version 120

uniform   float time;  //  Time
uniform   float arbi;  //  Strange arbitrary value
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
  for(float n=0; n<arbi; n++) {
    col += gl_Color/arbi;
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
