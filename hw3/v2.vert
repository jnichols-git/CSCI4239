//  Confetti Cannon V2
/*
 * This is a blatantly worse-looking version of V1, but curiously
 * it doesn't actually seem to have that bad of a performance
 * impact.
*/


#version 120

uniform   float time;  //  Time
attribute float start; //  Start time
attribute vec3  vel;   //  Velocity

float cycleTime() {
  return mod(time,5.0)-start;
}

bool hasStarted(float t) {
  return t<0.0;
}

vec4 position(vec4 vert, float t) {
  vert += t*vec4(vel,0);
  vert.y -= 4.9*t*t;
  return vert;
}

vec4 setParticle(vec4 vert, float t) {
  //  Pre-launch color black
  if (hasStarted(t))
     gl_FrontColor = vec4(0,0,0,1);
  else
  {
     //  Particle color
     gl_FrontColor = gl_Color;
     //  Particle trajectory
     vert = position(vert,t);
  }
  return vert;
}

void main(void)
{
   //  Particle location
   vec4 vert = gl_Vertex;
   //  Time offset mod 5
   float t = cycleTime();

   vert = setParticle(vert, t);
   //  Transform particle location
   gl_Position = gl_ModelViewProjectionMatrix*vert;
}
