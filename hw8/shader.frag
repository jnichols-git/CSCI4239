
varying vec4 ndc;
varying vec2 tex;
uniform sampler2D img;

void main() {
  // set frag color based on ndc, mapped from (-1,1) to (0,1)
  gl_FragColor = (ndc+1.0)/2.0;
}