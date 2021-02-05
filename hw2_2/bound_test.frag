
#version 120

varying float LightIntensity;

const vec4 col_body = vec4(0.0, 0.0, 1.0, 1.0);
const vec4 col_line = vec4(0.8, 0.8, 0.8, 1.0);

varying vec3 box_map; // space in bounding box
uniform int  stp; // step for calculations

float semicircle(float x) {
  return sqrt(1-pow(x,2));
}

void main() {
  // get texture coordinates
  vec2 tex_map = (gl_TexCoord[0].xy-.5)*2.0;
  // "inflate" texture map with box map
  tex_map.xy /= semicircle(box_map.x*0.9);
  tex_map.xy /= semicircle(box_map.z*0.9);
  tex_map.x += float(stp)/100.0;
  // multiply tex_map then take fractional part
  vec2 expanded = fract(tex_map*2.0);
  // if fractional part is >.8, set isBody to "true" i.e. 1.0
  vec2 isBody = step(expanded, vec2(.8,.8));
  // initialize color to red. you shouldn't see this.
  vec4 color = vec4(1.0,0.0,0.0,1.0);
  // create lines. x creates vertical, y creates horizontal
  if(isBody.xy == 1.0){
    color = col_body;
  } else {
    color = col_line;
  }
  gl_FragColor = color * LightIntensity;
}