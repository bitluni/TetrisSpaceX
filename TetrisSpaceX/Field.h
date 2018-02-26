#pragma once
#include "Sprites.h"

template<class Graphics>
class Field
{
  public:
  int field[18][10];
  bool full[18];
  
  Field()
  {
  }

  void start(int height = 5)
  {
    for(int y = 0; y < 18; y++)
    {
      for(int x = 0; x < 10; x++)
          field[y][x] = 0;
      full[y] = false;
    }
    for(int y = 0; y < height * 2; y++)
      for(int i = (rand() % 9) + 1; i >= 0; i--)
          field[17 - y][rand() % 10] = rand() % 11 + 1;
  }

  void draw(Graphics &g, Sprites<Graphics> &s, int dropFrame = -1)
  {
    for(int y = 0; y < 18; y++)
    {
      if(full[y])
      {
        if(dropFrame < 10) continue;
        for(int x = 0; x < 10; x++)
          s.draw(g, x * 8 + 16, y * 8, (dropFrame % 10 >= 5) ? 14 : field[y][x]);
      }
      else
      for(int x = 0; x < 10; x++)
        s.draw(g, x * 8 + 16, y * 8, field[y][x]);
    }    
  }

  void set(int x, int y, int b)
  {
    field[y][x] = b;
    int c = 0;
    for(int x = 0; x < 10; x++)
      if(field[y][x] > 0) c++;
    if(c == 10) full[y] = true;
  }

  bool hasFull()
  {
    for(int y = 0; y < 18; y++)
      if(full[y]) return true;
    return false;
  }

  bool blocked(int x, int y)
  {
    if(x < 0 || x >= 10 || y < 0 || y >= 18)
      return true;
    return field[y][x] != 0;
  }

  int drop()
  {
    int dropped = 0;
    int t = 17;
    int source[18];
    for(int y = 17; y >= 0; y--)
    {
      source[y] = 0;
      if(!full[y])
        source[t--] = y;
      else
        dropped++;
    }
    for(int y = 17; y >= 0; y--)
    {
      full[y] = false;
      for(int x = 0; x < 10; x++)
        field[y][x] = field[source[y]][x];
    }
    return dropped;
  }
};

