//  Confetti Cannon
//  derived from Orange Book
#version 120


void main(void)
{
   gl_FrontColor = gl_Color;
   gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;
}
