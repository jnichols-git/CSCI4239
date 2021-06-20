// Shift height based on input texture
#version 400 core

// Triangle in, triangle out
layout(triangles, invocations = 1) in;
layout(triangle_strip, max_vertices=3) out;

// Matrices from cpp
uniform mat4 ModelView;
uniform mat4 Projection;

// Time
uniform float time;
uniform int count;
uniform float P;  // Propogation
uniform float W;  // Wavelength
uniform float M;  // Magnitude
float C = 10.0; // Cycle
float PI = 3.1415;

// Texture
uniform sampler3D ripple;

// In from VS
in VS_GS_INTERFACE {
  in vec4 vCol; // Color
  in vec2 vTex; // Tex Coord
} gs_in[];

// Out to FS
out GS_FS_INTERFACE {
  out vec4 vCol; // Color
  out vec2 vTex; // TexCoord
} gs_out;

float wave_height(vec2 tc, float layer) {
  vec4 pix = texture(ripple, vec3(tc, layer));
  float center = pix.r*C + (1-pix.g)*P; // Center time of cosine wave
  // Check bounds of wave
  float t     = mod(time,C);
  float bound = abs(t-center);
  if(bound > pix.b*W) return 0;
  // Otherwise, find wave height
  else {
    float h = .5*cos((PI*(t-center))/(pix.b*W))+.5;
    return h*pix.a*M;
  }
}

void main(void) {
  mat4 MVP = Projection * ModelView;
  for(int n=0; n<gl_in.length(); n++) {
    vec4 pos = gl_in[n].gl_Position;

    float h = 0.0;
    // Select greatest height from a series of waves
    float incr = 1.0/float(count);
    for(float i=0.0; i<1.0; i+=incr) {
      float wave = wave_height(gs_in[n].vTex, i);
      if(wave > h) h = wave;
    }
    pos.y += h;

    gl_Position = MVP * pos;
    gs_out.vCol = gs_in[n].vCol - pos.y;
    gs_out.vTex = gs_in[n].vTex;
    EmitVertex();
  }
  EndPrimitive();
}