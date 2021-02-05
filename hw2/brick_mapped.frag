//  Brick fragment shader
//  Derived from Orange Book Chapter 6 by Charles Gruenwald
#version 120

//  Hardwire brick dimensions and color
const vec3 BrickColor  = vec3(1.0,0.3,0.2);
const vec3 MortarColor = vec3(0.85,0.86,0.84);
const vec2 BrickSize   = vec2(0.30,0.15);
const vec2 BrickPct    = vec2(0.90,0.85);
const float pi = 3.145927;

//  Model coordinates and light from vertex shader
varying float LightIntensity;
varying vec2  ModelPos;
uniform float time;
uniform float rotation;

// input from x_mapped vert shader
varying vec4 obj_coords;

void main()
{
  //  Generate spatial coordinate from texture coordinates
  float r = (gl_TexCoord[0].x-0.5)*pi;
  // Alter brick size according to position in texture
  vec2 RadSize = BrickSize*cos(gl_FragCoord.x/1000.0);
   //  Divide by brick size for unit coordinates
   vec2 position = ModelPos / RadSize;
   //  Stagger rows
   if (fract(0.5*position.y)>0.5) position.x += 0.5;
   //  Scroll bricks at 2.5 bricks per second
   position.x += 0.1*rotation;
   //  Don't care about the integer part
   position = fract(position);
   //  Snap to 0 or 1
   vec2 useBrick = step(position,BrickPct);
   //  Interpolate color (0 or 1 gives sharp transition)
   vec3 color  = mix(MortarColor, BrickColor, useBrick.x*useBrick.y);
   //  Adjust color intensity for lighting (interpolated from vertex shader values)
   color *= LightIntensity;
   //  Pad color with alpha
   gl_FragColor = vec4(color,1.0);
}
