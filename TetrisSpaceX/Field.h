#pragma once
#include "Sprites.h"

class Field
{
  public:
  int field[18][10];
  bool full[18];
  
  Field()
  {
  }

  void addLines(int count)
  {
    for(int y = 0; y < 18 - count; y++)
    {
      for(int x = 0; x < 10; x++)
        field[y][x] = field[y + count][x];
      full[y] = full[y + count];      
    }
    for(int y = 0; y < count; y++)
    {
      for(int x = 0; x < 10; x++)
          field[17 - y][x] = 0;
      for(int i = (rand() % 9) + 1; i >= 0; i--)
          field[17 - y][rand() % 10] = rand() % 11 + 1;
      full[17 - y] = false;
    }
  }

  void start(int high = 0)
  {
    for(int y = 0; y < 18; y++)
    {
      for(int x = 0; x < 10; x++)
          field[y][x] = 0;
      full[y] = false;
    }
    addLines(high * 2);
  }

  void draw(Graphics &g, Sprites &s, int x0, int y0, int dropFrame = -1, int solid = 18)
  {
    for(int y = 0; y < 18; y++)
    {
      if(solid <= y)
        for(int x = 0; x < 10; x++)
          s.draw(g, 13, x0 + x * 8 + 16, y0 + y * 8);
      else      
        if(full[y])
        {
          if(dropFrame < 10) continue;
          for(int x = 0; x < 10; x++)
            s.draw(g, (dropFrame % 10 >= 5) ? 14 : field[y][x], x0 + x * 8 + 16, y0 + y * 8);
        }
        else
        for(int x = 0; x < 10; x++)
          s.draw(g, field[y][x], x0 + x * 8 + 16, y0 + y * 8);
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

  int hasFull()
  {
    int f = 0;
    for(int y = 0; y < 18; y++)
      if(full[y]) f++;
    return f;
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

