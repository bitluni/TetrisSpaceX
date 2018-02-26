#pragma once

int nextRand;
unsigned int rand()
{ 
   nextRand = nextRand * 1103515245 + 12345; 
   return ((unsigned int)(nextRand / 65536) % 32768); 
}
 
void srand(unsigned int seed) 
{ 
   nextRand = seed; 
}
