#version 120

const vec2 center = vec2(1.0/3.0,2.0/3.0); // hole dimensions
const vec4 outerColor = vec4(.8,.8,.8,1.0);
const vec4 innerColor = vec4(.6,.6,.6,1.0);

uniform int iterations;
const int fractal_factor = 3; // rate at which fractal splits
int fractal_div = 1; // divisions of fractal

varying float LightIntensity;

// returns >=1.0 if p.x *XOR* p.y is in range r
float inRange_xor(vec2 p, vec2 r) {
  return abs(step(r.x,p.x)*step(p.x,r.y)-step(r.x,p.y)*step(p.y,r.y));
}

void main() {
  // get tex coords
  vec2 coords = gl_TexCoord[0].xy;
  // set default color
  if(gl_FrontFacing){
    gl_FragColor=outerColor;
  } else {
    gl_FragColor=innerColor;
  }
  // iterate through fractal
  for(int n=0; n<iterations; n++) {
    if(inRange_xor(fract(coords*fractal_div), center) >= 1.0) {
      discard;
    }
    fractal_div *= fractal_factor;
  }
  // add lighting
  gl_FragColor *= LightIntensity;
}