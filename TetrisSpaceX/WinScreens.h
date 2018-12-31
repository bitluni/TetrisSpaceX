void dance(int t)
{
  if(t == 0)
    playMusic(1, false);
  int x = (graphics.xres - 160) / 2;
  int y = (graphics.yres - 144) / 2;
  screens.draw(graphics, 5, x, y);
  if((t / 10) & 1)
  {
    winScreens.draw(graphics, 0, x, y);
    winScreens.draw(graphics, 2, x, y);
  }
  else
  {
    winScreens.draw(graphics, 1, x, y);
    winScreens.draw(graphics, 3, x, y);
  }
  graphics.setCursor(x + 112, 8);
  graphics.setTextColor(0, -1);
  graphics.print("LEVEL\n");
  graphics.print(games[0]->level, 10, 3);
  graphics.print("\n\nHIGH\n");
  graphics.print(games[0]->high, 10, 3);  
  graphics.print("\n\n\n\nLINES\n");
  graphics.print(games[0]->lines, 10, 4);
}

void start(int t)
{
  if(t == 0)
  {
    stopMusic();
    playMusic(3);
  }
  int x = (graphics.xres - 160) / 2;
  int y = (graphics.yres - 144) / 2;
      
  int ry = (t - 50) / 2;
  int ym = max(ry, 0);
  ym = ym * ym / 100;
  graphics.fillRect(x, y, 160, 144, 54);
  winScreens.draw(graphics, 4, x, y);
  if(t < 60)
    winScreens.draw(graphics, 5, x, y);
  else
    winScreens.draw(graphics, 6, x, y);

  int x0 = winScreens.point(4, 1)[0] - winScreens.point(4, 0)[0];
  int y0 = winScreens.point(4, 1)[1] - winScreens.point(4, 0)[1];
  if(t > 30 && t < 90 && (t / 2 & 1))
    rocket.draw(graphics, (t < 50 || t > 71) ? 11 : 12, x + x0, y + y0);

  rocket.draw(graphics, 0, x + x0, y + y0 - ym);
  if(t > 50)
  {
    rocket.draw(graphics, 13 + (rand() & 1), x + x0, y + y0 - ym);
    rocket.draw(graphics, 13 + (rand() & 1), x + x0 - rocket.xres(14) + 1, y + y0 - ym);
    rocket.draw(graphics, 13 + (rand() & 1), x + x0 + rocket.xres(14) - 1, y + y0 - ym);
  }
  if(t == 30)
  {
    sounds.play(audioSystem, 12, 0.3, 1, true);
  }
  if(t == 40)
  {
    sounds.stop(audioSystem);
    sounds.play(audioSystem, 12, 0.3, 1, true);
    sounds.play(audioSystem, 12, 0.3, 1.5, true);        
  }
  if(t == 50)
  {
    sounds.stop(audioSystem);
    sounds.play(audioSystem, 12, 0.3, 1.5, true);        
    sounds.play(audioSystem, 12, 0.3, 1.6, true);
    sounds.play(audioSystem, 12, 0.3, 1.7, true);        
  }
}

void landing1(int t)
{
  if(t == 0)
  {
    sounds.stop(audioSystem);
    sounds.play(audioSystem, 12, 0.3, 1.8, true);
    sounds.play(audioSystem, 12, 0.3, 2.0, true);
  }
  int x = (graphics.xres - 160) / 2;
  int y = (graphics.yres - 144) / 2;
  graphics.fillRect(x, y, 160, 144, 54);
  winScreens.draw(graphics, 7, x, y);
  
  int ry = (250 - t) / 2;
  int ym = max(ry, 0);
  ym = ym * ym / 100;      
  int ry2 = (270 - t) / 2;
  int y2m = max(ry2, 0);
  y2m = y2m * y2m / 100;     

  const short (*p)[2] = winScreens.points(7);
      
  rocket.draw(graphics, 1 + max(3 - max(ym - 20, 0) / 2, 0), x + p[1][0] - p[0][0], y + p[1][1] - p[0][1] - ym);
  rocket.draw(graphics, 1 + max(3 - max(y2m - 20, 0) / 2, 0), x + p[2][0] - p[0][0], y + p[2][1] - p[0][1] - y2m);

  if(ry > 0)   
    rocket.draw(graphics, 13 + (rand() & 1), x + p[1][0] - p[0][0], y + p[1][1] - p[0][1] - ym);
  if(ry2 > 0)   
    rocket.draw(graphics, 13 + (rand() & 1), x + p[2][0] - p[0][0], y + p[2][1] - p[0][1] - y2m);

  if(t == 250)
  {
    sounds.stop(audioSystem);
    sounds.play(audioSystem, 12, 0.3, 2.0, true);
  }
  if(t == 270)
    sounds.stop(audioSystem);

  int s = (ry + 20) / 20;
  int s2 = (ry2 + 20) / 20;
  if(s < 3 && s > -1 && (t / 2 & 1))
    rocket.draw(graphics, (s < 1 || s > 1) ? 11 : 12, x + p[1][0] - p[0][0], y + p[1][1] - p[0][1]);
  if(s2 < 3 && s2 > -1 && (t / 2 & 1))
    rocket.draw(graphics, (s2 < 1 || s2 > 1) ? 11 : 12, x + p[2][0] - p[0][0], y + p[2][1] - p[0][1]);
}

void landing2(int t)
{
  if(t == 0)
  {
    sounds.stop(audioSystem);
    sounds.play(audioSystem, 12, 0.3, 2.0, true);
  }
  if(t == 250)
    sounds.stop(audioSystem);
  int x = (graphics.xres - 160) / 2;
  int y = (graphics.yres - 144) / 2;
  graphics.fillRect(x, y, 160, 144, 54);

  winScreens.draw(graphics, 8 + ((t / 4) & 3), x, y);  

  int ry = (250 - t) / 2;
  int ym = max(ry, 0);
  ym = ym * ym / 100;
  if(t < 250)
  {
    const short (*p)[2] = winScreens.points(8);  
    int x0 = p[1][0] - p[0][0];
    int y0 = p[1][1] - p[0][1];
    rocket.draw(graphics, 5 + max(3 - max(ym - 20, 0) / 2, 0), x + x0, y + y0 - ym);
    rocket.draw(graphics, 13 + (rand() & 1), x + x0, y + y0 - ym);
    int s = (ry + 20) / 20;
    if(s < 3 && s > -1 && (t / 2 & 1))
      rocket.draw(graphics, (s < 1 || s > 1) ? 11 : 12, x + x0, y + y0);
  }
  else
  {
    const short (*p)[2] = winScreens.points(8 + ((t / 4) & 3));  
    int x0 = p[1][0] - p[0][0];
    int y0 = p[1][1] - p[0][1];
    const int b2[] = {8, 9, 8, 10};
    rocket.draw(graphics, b2[((t / 4) & 3)], x + x0, y + y0);
    int s = (ry + 20) / 20;
    if(s < 3 && s > -1 && (t / 2 & 1))
      rocket.draw(graphics, (s < 1 || s > 1) ? 11 : 12, x + x0, y + y0);
  }
}

void mars(int t)
{
  if(t == 0)
    sounds.stop(audioSystem);
  int tmax = t / 8 > 80 ? 80 * 8 : t;
  
  int x = (graphics.xres - 320) / 2;
  int y = (graphics.yres - 144) / 2;
  winScreens.draw(graphics, 12, x - 80 + tmax / 8, y);
  rocket.draw(graphics, 16, x + 320 - t / 2, y + 80 + t / 8);
  rocket.draw(graphics, 15, x - 100 + t, 150 - t / 4);
      
  graphics.setTextColor(40, -1);
  graphics.setCursor(graphics.xres / 2 - 8 * 8, 24);
  char d[] = "CONGRATULATIONS!";
  char c[17];
  int i = 0;
  for(;i < min(((t - 100) / 2), 16); i++)
    c[i] = d[i];
  c[i] = 0;
  graphics.print(c);  
}

