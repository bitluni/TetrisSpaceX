#pragma once
#include "Image.h"

template<class Graphics>
class Sprites: public Image<Graphics>
{
  public:
  int spriteYres;
  
  Sprites(int spriteHeight, int xres_, int yres_, const unsigned char *pixels_)
    :Image<Graphics>(xres_, yres_, pixels_),
    spriteYres(spriteHeight)
  {
  }

  void draw(Graphics &g, int x, int y, int sprite)
  {
    Image<Graphics>::draw(g, x, y, 0, sprite * spriteYres, Image<Graphics>::xres, spriteYres);
  }
};

