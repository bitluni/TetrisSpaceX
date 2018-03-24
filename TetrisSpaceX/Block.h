#pragma once
#include "Field.h"
#include "Sprites.h"

class Block
{
  public:
  int type;
  int xPos;
  int yPos;
  int orientation;
  bool active;
  Sprites *sprites;

  Block(){}
  
  void init(Sprites &blockSprites)
  {
    active = false;
    sprites = &blockSprites;    
  }
  
  void getNext()
  {
    type = rand() % 6;
    orientation = 0;    
    xPos = 4;
    yPos = 2;
    active = false;    
  }

  const char *getParts(int &x, int &y, int &xres, int &yres, int o)
  {
    switch(type)
    {
      case 0:
        {
          x = -1;
          y = -2;
          xres = 4;
          yres = 4;
          static const char longOne[][16] = {{0,0,0,0, 0,0,0,0, 9,10,10,11, 0,0,0,0}, {0,6,0,0, 0,7,0,0, 0,7,0,0, 0,8,0,0}};
          return longOne[o & 1];
        }
      case 1:
        {
          x = 0;
          y = -1;
          xres = 2;
          yres = 2;
          static const char quad[] = {2,2,2,2};
          return quad;               
        }
      case 2:
        {
          x = -1;
          y = -1;
          xres = 3;
          yres = 3;
          static const char t[][9] = {{0,0,0, 5,5,5, 0,5,0}, {0,5,0, 5,5,0, 0,5,0}, {0,5,0, 5,5,5, 0,0,0}, {0,5,0, 0,5,5, 0,5,0}};
          return t[o];               
        }
      case 3:
        {
          x = -1;
          y = -1;
          xres = 3;
          yres = 3;
          static const char l[][9] = {{0,0,0, 3,3,3, 0,0,3}, {0,3,0, 0,3,0, 3,3,0}, {3,0,0, 3,3,3, 0,0,0}, {0,3,3, 0,3,0, 0,3,0}};
          return l[o];               
        }
      case 4:
        {
          x = -1;
          y = -1;
          xres = 3;
          yres = 3;
          static const char l[][9] = {{0,0,0, 12,12,12, 12,0,0}, {12,12,0, 0,12,0, 0,12,0}, {0,0,12, 12,12,12, 0,0,0}, {0,12,0, 0,12,0, 0,12,12}};
          return l[o];               
        }
      case 5:
        {
          x = -1;
          y = -2;
          xres = 3;
          yres = 3;
          static const char s[][9] = {{0,0,0, 0,4,4, 4,4,0}, {4,0,0, 4,4,0, 0,4,0}};
          return s[o & 1];               
        }
      case 6:
        {
          x = -1;
          y = -2;
          xres = 3;
          yres = 3;
          static const char s[][9] = {{0,0,0, 1,1,0, 0,1,1}, {0,0,1, 0,1,1, 0,1,0}};
          return s[o];               
        }
    }
  }

  void start()
  {
    active = true;
  }

  void draw(Graphics &g, int x0, int y0)
  {
    int dx;
    int dy;
    int xres;
    int yres;
    const char *parts = getParts(dx, dy, xres, yres, orientation);
    int bx = dx * 8;
    int by = dy * 8;
    if(!active)
    {
      bx += 128;
      by += 112;
    }
    else
    {
      bx += xPos * 8 + 16;
      by += yPos * 8;      
    }
    for(int y = 0; y < yres; y++)
      for(int x = 0; x < xres; x++)
        if(parts[y * xres + x])
          sprites->draw(g, parts[y * xres + x], x0 + bx + x * 8, y0 + by + y * 8);
  }

  bool test(Field &field, int xt, int yt, int o)
  {
    int dx;
    int dy;
    int xres;
    int yres;
    const char *parts = getParts(dx, dy, xres, yres, o);
    for(int y = 0; y < yres; y++)
      for(int x = 0; x < xres; x++)
        if(parts[y * xres + x] && field.blocked(x + xt + dx, y + yt + dy))
          return true;
    return false;
  }
  
  void place(Field &field)
  {
    int dx;
    int dy;
    int xres;
    int yres;
    const char *parts = getParts(dx, dy, xres, yres, orientation);
    for(int y = 0; y < yres; y++)
      for(int x = 0; x < xres; x++)
        if(parts[y * xres + x])
          field.set(x + xPos + dx, y + yPos + dy, parts[y * xres + x]);        
    active = false;            
  }
  
  void down()
  {
    yPos++;    
  }

  void cw()
  {
    orientation = (orientation + 1) & 3;
  }

  void ccw()
  {
    orientation = (orientation - 1) & 3;    
  }
  
  void left()
  {
    xPos--;
  }

  void right()
  {
    xPos++;
  }
  
  void operator=(Block &b)
  {
    type = b.type;
    active = true;
    xPos = b.xPos;
    yPos = b.yPos;
    orientation = b.orientation;
    b.getNext();
  }
};

