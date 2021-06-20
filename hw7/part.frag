// Sample texture to determine if pixel is active
#version 120

uniform sampler3D noise;
uniform float time;

float cycle = 60.0; // How often the particles refresh
float range = 1.0;  // Time range +- for particle activity

void main() {
  vec4 point = texture3D(noise, gl_TexCoord[0].xyz);
  vec4 color = vec4(point.rgb, 1.0);
  float i = point.a;          // Noise intensity (grayscale, rgb are equal)
  float t = mod(time, cycle); // Time point in cycle


  if(i==0.0) discard;

  float part = range - abs(t-cycle*i);
  if(part > 0.0) {
    gl_FragColor = color * (part/range);
  } else {
    discard;
  }

  //gl_FragColor = point;
}