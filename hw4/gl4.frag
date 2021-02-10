#version 400 core

//  Input from previous shader
in GS_FS_INTERFACE {
  in vec3 vNrm;
  in vec4 vCol;
  in vec2 vTex;
} fs_in;
//  Fragment color
layout (location=0) out vec4 Fragcolor;

//  Texture
uniform sampler2D tex;

void main()
{
   Fragcolor = fs_in.vCol*texture2D(tex,fs_in.vTex);
}
