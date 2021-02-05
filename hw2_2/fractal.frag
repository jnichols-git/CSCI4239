#version 120

const vec2 center = vec2(1.0/3.0,2.0/3.0); // hole dimensions
const vec4 outerColor = vec4(.8,.8,.8,1.0);
const vec4 innerColor = vec4(.6,.6,.6,1.0);

uniform int iterations;
int fractal_div = 1; // divisions of fractal

// check if point p is in range r
// returns 1.0 if the value is in range
float inRange(vec2 p, vec2 r) {
  return step(r.x,p.x)*step(p.x,r.y)*step(r.x,p.y)*step(p.y,r.y);
}

void main() {
  // get tex coords
  vec2 coords = gl_TexCoord[0].xy;
  for(int n=0; n<iterations; n++) {
    if(inRange(fract(coords*fractal_div), center) == 1.0) {
      discard;
    }
    fractal_div *= 3;
  }
  if(gl_FrontFacing){
    gl_FragColor=outerColor;
  } else {
    gl_FragColor=innerColor;
  }
}