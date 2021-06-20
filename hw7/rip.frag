// boy howdy that sure is a shader that does something
#version 400 core

// In from GS
in GS_FS_INTERFACE {
  in vec4 vCol; // Color
  in vec2 vTex; // TexCoord
} fs_in;

// Output color
layout(location=0) out vec4 FragColor;

uniform sampler3D ripple;

void main(void) {
  FragColor = fs_in.vCol*texture(ripple,vec3(fs_in.vTex, 0.0));
}