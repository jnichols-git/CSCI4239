//  Particle reflection shader
#version 120

uniform   float time;  //  Time
uniform   float cycle; // Cycle time
uniform   float angle; // Angle of reflection plane
attribute float STR; //  Start time
attribute vec3  VEL;   //  Velocity

float g = 9.81;

// Returns distance under plane if below, or 0 if not
float underPlane(vec4 pos) {
  // Bounding coordinates based on rotation
  vec3 bounds = vec3(cos(radians(angle)), sin(radians(angle)), 1.0);
  // Check bounds
  if(abs(pos.x) >= bounds.x) return 0;
  if(abs(pos.z) >= bounds.z) return 0;
  float dist = (bounds.y*(pos.x/bounds.x)) - pos.y;
  if(dist > 0) return dist;
  return 0;
}

// Approximate contact time from height
float contactTime(float dist, float vel, float accel) {
  // quadratic: (1/2a)t^2 + vt^2 + d
  float t = -vel + sqrt(pow(vel,2)-2*accel*dist);
  t /= 2*accel;
  return t;
}

// Transform point based on initial position, velocity, acceleration, and time
vec4 transform(vec4 p0, vec4 v0, vec4 a, float t) {
  vec4 p1 = p0 + v0*t + .5*a*t*t;
  // Distance below plane
  float under = underPlane(p1);
  // Reflect if under plane
  // It may have been a while since I've done physics...
  if(under>0) {
    float dh = (p1.y + under) - p0.y;
    // Estimate time of contact based on height with quadratic formula
    float ct = (-v0.y - sqrt(pow(v0.y,2)+2*a.y*dh))/a.y;

    // Reflect about normal starting at time ct
    float dt = t-ct;
    vec3  normal = vec3(-sin(radians(angle)),cos(radians(angle)),0);
    vec3  v_ref = reflect(vec3(v0.x,v0.y,v0.z),normal);
    vec4  dv = vec4(v_ref, 0);
    // Amplify dv by collion speed
    dv *= -v0.y-a.y*ct;
    // Point of collision
    p1 = p0 + v0*ct + .5*a*ct*ct;
    // Calculated from point
    p1 = p1 + dv*dt + .5*a*dt*dt;
  }
  return p1;
}

void main(void)
{
   //  Particle location
   vec4 vert = gl_Vertex;
   //  Time offset mod 10
   float t = mod(time,cycle)-(STR*cycle);

   //  Pre-launch color black
   if (t<0.0)
      gl_FrontColor = vec4(0,0,0,1);
   else
   {
      //  Particle color
      gl_FrontColor = gl_Color;
      //  Particle trajectory
      vert = transform(vert, vec4(VEL,0), vec4(0,-g,0,0), t);
   }
   //  Transform particle location
   gl_Position = gl_ModelViewProjectionMatrix*vert;
}
