#include "CSCIx239.h"

//
//  Frames per seconds
//
static int fps=0,sec0=0,count=0,avg=0;
int prev_fps[100]; int next_idx=0;
int FramesPerSecond(void)
{
   int sec = glfwGetTime();
   if (sec!=sec0)
   {
      sec0 = sec;
      fps = count;
      prev_fps[next_idx] = fps; next_idx++;
      next_idx %= 100;
      count=0;
   }
   count++;
   return fps;
}

int AvgFPS(void) {
  FramesPerSecond();
  int sum=0,ct=0;
  for(int n=0; n<100; n++) {
    if(prev_fps[n]!=0){
      sum+=prev_fps[n]; avg/=2;
      ct++;
    }
  }
  if(ct>=1) avg=sum/ct;
  return avg;
}

void ResetAvg(void) {
  for(int n=0; n<100; n++) {
    prev_fps[n] = 0;
  }
  next_idx=0;
}
