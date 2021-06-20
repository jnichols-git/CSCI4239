#version 120

uniform sampler2D sprite;

void main() {
  if(gl_Color.a == 0.0) discard;
  vec4 color = texture2D(sprite, gl_PointCoord.st);
  color.a = .8;
  gl_FragColor = gl_Color*color;
}