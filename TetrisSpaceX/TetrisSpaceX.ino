//code by bitluni (send me a high five if you like the code)

#include "CompositeGraphics.h"
#include "Image.h"
#include "Sprites.h"
#include "CompositeOutput.h"
#include "Block.h"
#include "Field.h"
#include "GameControllers.h"
#include "sound.h"

////////////////////////////
//pick your controller types
const int LATCH = 16;
const int CLOCK = 17;
const GameControllers::Type PLAYER1_TYPE = GameControllers::SNES;
const int DATA_PLAYER1 = 18;
const GameControllers::Type PLAYER2_TYPE = GameControllers::NES;
const int DATA_PLAYER2 = 19;

//tons of uncleaned code
#include "includes.h"

//PAL MAX, half: 324x268 full: 648x536
//NTSC MAX, half: 324x224 full: 648x448
const int XRES = 160;
const int YRES = 144;

CompositeGraphics graphics(XRES, YRES);
CompositeOutput composite(CompositeOutput::NTSC, XRES * 2, YRES * 2);

Image<CompositeGraphics> gameScreenB(image0::xres, image0::yres, image0::pixels);
Image<CompositeGraphics> danceScreen0(dance0::xres, dance0::yres, dance0::pixels);
Image<CompositeGraphics> danceScreen1(dance1::xres, dance1::yres, dance1::pixels);
Image<CompositeGraphics> launchScreen0(launch0::xres, launch0::yres, launch0::pixels);
Image<CompositeGraphics> launchScreen1(launch1::xres, launch1::yres, launch1::pixels);
Image<CompositeGraphics> rocket00(rocket0::xres, rocket0::yres, rocket0::pixels);
Image<CompositeGraphics> rocket01(rocket1::xres, rocket1::yres, rocket1::pixels);
Image<CompositeGraphics> rocket02(rocket2::xres, rocket2::yres, rocket2::pixels);
Image<CompositeGraphics> steam00(steam0::xres, steam0::yres, steam0::pixels);
Image<CompositeGraphics> steam01(steam1::xres, steam1::yres, steam1::pixels);
Sprites<CompositeGraphics> sprites8(8, sprites0::xres, sprites0::yres, sprites0::pixels);
Font<CompositeGraphics> font(8, 8, tetrisFont::pixels);
Image<CompositeGraphics> booster00(boostera1::xres, boostera1::yres, boostera1::pixels);
Image<CompositeGraphics> booster01(boostera2::xres, boostera2::yres, boostera2::pixels);
Image<CompositeGraphics> booster02(boostera3::xres, boostera3::yres, boostera3::pixels);
Image<CompositeGraphics> booster03(boostera4::xres, boostera4::yres, boostera4::pixels);
Image<CompositeGraphics> booster10(boosterb1::xres, boosterb1::yres, boosterb1::pixels);
Image<CompositeGraphics> booster11(boosterb2::xres, boosterb2::yres, boosterb2::pixels);
Image<CompositeGraphics> booster12(boosterb3::xres, boosterb3::yres, boosterb3::pixels);
Image<CompositeGraphics> booster13(boosterb4::xres, boosterb4::yres, boosterb4::pixels);

Image<CompositeGraphics> waters0(water0::xres, water0::yres, water0::pixels);
Image<CompositeGraphics> waters1(water1::xres, water1::yres, water1::pixels);
Image<CompositeGraphics> waters2(water2::xres, water2::yres, water2::pixels);
Image<CompositeGraphics> waters3(water3::xres, water3::yres, water3::pixels);
Image<CompositeGraphics> waters4(water4::xres, water4::yres, water4::pixels);
Image<CompositeGraphics> waters5(water5::xres, water5::yres, water5::pixels);
Image<CompositeGraphics> waters6(water6::xres, water6::yres, water6::pixels);
Image<CompositeGraphics> waters7(water7::xres, water7::yres, water7::pixels);

Image<CompositeGraphics> flames0(flame0::xres, flame0::yres, flame0::pixels);
Image<CompositeGraphics> flames1(flame1::xres, flame1::yres, flame1::pixels);

Image<CompositeGraphics> landings(landing::xres, landing::yres, landing::pixels);
Image<CompositeGraphics> mars0(mars::xres, mars::yres, mars::pixels);
Image<CompositeGraphics> stars0(stars::xres, stars::yres, stars::pixels);
Image<CompositeGraphics> starman0(starman::xres, starman::yres, starman::pixels);
Image<CompositeGraphics> branding0(branding::xres, branding::yres, branding::pixels);

#include <soc/rtc.h>

GameControllers controllers;

volatile int frame = 0;
Block<CompositeGraphics> nextBlock(sprites8);
Block<CompositeGraphics> currentBlock(sprites8);
Field<CompositeGraphics> field;
int gameLost = 18;
int timeToStep;
int stepTicks[] = {17, 16, 15, 14, 13, 12, 11, 10, 8, 6};
int lines = 25;
int score = 0;
int high = 5;
int level = 9;
int directionDelay = 0;
int directionTicks = 4;
int dropping = -1;
int gameWon = -1;

void setup()
{
  rtc_clk_cpu_freq_set(RTC_CPU_FREQ_240M);              //highest cpu frequency
  composite.init();
  graphics.init();
  graphics.setFont(font);
  xTaskCreatePinnedToCore(compositeCore, "c", 1024, NULL, 1, NULL, 0);
  srand(1337);
  field.start();
  nextBlock.getNext();
  controllers.init(LATCH, CLOCK);
  controllers.setController(0, GameControllers::SNES, DATA_PLAYER1);
  controllers.setController(1, GameControllers::NES, DATA_PLAYER2);
  initSound();
}

void compositeCore(void *data)
{
  while (true)
  {
    composite.sendFrameHalfResolution(&graphics.frame);
    frame++;
  }
}

void startGame()
{
  gameLost = -1;
  field.start(high);
  nextBlock.getNext();
  currentBlock.active = false;
  lines = 25;
  level = 9;
  score = 0;
  timeToStep = 30;
  directionDelay = 0;
  dropping = -1;
  gameWon = -1;
  playing = true;
  sample = 0;
}

void printText()
{
  graphics.setCursor(112, 8);
  graphics.setTextColor(0, -1);
//  graphics.print("SCORE\n\n\n\n\nLEVEL\n\n\nLINES");
  graphics.print("LEVEL\n");
  graphics.print(level, 10, 3);
  graphics.print("\n\nHIGH\n");
  graphics.print(high, 10, 3);  
  graphics.print("\n\n\n\nLINES\n");
  graphics.print(lines, 10, 4);
}

void drawGameScreen()
{
  gameScreenB.draw(graphics, 0, 0);
  printText();
  field.draw(graphics, sprites8, dropping);
  nextBlock.draw(graphics);   
}

void draw()
{
  Image<CompositeGraphics> *b[] = {&booster00, &booster01, &booster02, &booster03};
  Image<CompositeGraphics> *b2[] = {&booster10, &booster11, &booster12, &booster13};
  Image<CompositeGraphics> *s[] = {&steam00, &steam01, &steam00};
  Image<CompositeGraphics> *f[] = {&flames0, &flames1};
  Image<CompositeGraphics> *r[] = {&rocket00, &rocket01, &rocket02};
  Image<CompositeGraphics> *w[] = {&waters0, &waters1, &waters2, &waters3, &waters4, &waters5, &waters6, &waters7};
  static int lastFrameMillis = 0;
  int thisFrameMillis = millis();
  if(thisFrameMillis - lastFrameMillis < (1000 / 30)) return;
  lastFrameMillis = thisFrameMillis;
  controllers.poll();
  if(controllers.pressed(0, GameControllers::X))
    gameWon = 0;
  if(controllers.pressed(0, GameControllers::START))
    startGame();
  if(controllers.pressed(0, GameControllers::L))
    level = level ? level - 1 : 0;
  if(controllers.pressed(0, GameControllers::R))
    level = (level < 9) ? level + 1 : 9;
  if(controllers.pressed(0, GameControllers::SELECT))
    high = (high + 1) % 6;
    
  graphics.begin(true);
  if(gameWon > -1)
  {
    playing = false;
    if(gameWon < 1000)
    {
      //drawGameScreen();
      if((gameWon / 10) & 1)
        danceScreen0.draw(graphics, 0, 0);
      else
        danceScreen1.draw(graphics, 0, 0);
      printText();
      if(controllers.pressed(0, GameControllers::Y)) gameWon = 1000;
    }
    else
    if(gameWon < 1350)
    {
      int t = gameWon - 1000;
      int y = (t - 50) / 2;
      int ym = max(y, 0);
      ym = ym * ym / 100;
      if(t < 50)
        launchScreen0.draw(graphics, 0, 0, 0, 0, 160, 144);
      else
        launchScreen1.draw(graphics, 0, 0, 0, 0, 160, 144);
      r[y < 0 ? 0 : (rand() & 1) + 1]->draw(graphics, 69, 3 - ym, 3);
      if(y < 40 && y > -20 && (gameWon / 2 & 1))
        s[(y + 20) / 20]->draw(graphics, 49, 98, 3);
      if(controllers.pressed(0, GameControllers::Y)) gameWon = 1350;
      /*if(t > 320)
        branding0.draw(graphics, 0, 0, 0);*/
    }
    else
    if(gameWon < 1750)
    {
      int t = gameWon - 1350;
      int y = (300 - t) / 2;
      int ym = max(y, 0);
      ym = ym * ym / 100;      
      int y2 = (310 - t) / 2;
      int y2m = max(y2, 0);
      y2m = y2m * y2m / 100;     
      if(t * 16 < 160) launchScreen1.draw(graphics, 0, 0);      
      landings.draw(graphics, 0, 0, 0, 0, min(t * 16, 160), 144);
      b[max(3 - max(ym - 20, 0) / 2, 0)]->draw(graphics, 28, 27 - ym, 3);
      b[max(3 - max(y2m - 20, 0) / 2, 0)]->draw(graphics, 112, 29 - y2m, 3);
      if(y > 0)   
          f[rand() & 1]->draw(graphics, 28, 27 - ym, 3);
      if(y2 > 0)
          f[rand() & 1]->draw(graphics, 112, 29 - y2m, 3);
      if(y < 40 && y > -20 && (gameWon / 2 & 1))
        s[(y + 20) / 20]->draw(graphics, 9, 122, 3);
      if(y2 < 40 && y2 > -20 && (gameWon / 2 & 1))
        s[(y2 + 20) / 20]->draw(graphics, 92, 124, 3);
      if(controllers.pressed(0, GameControllers::Y)) gameWon = 1800;
      /*if(t > 400)
        branding0.draw(graphics, 0, 0, 0);   */   
    }
    else
    if(gameWon < 2150)
    {
      int t = gameWon - 1750;
      int y = (300 - t) / 2;
      int ym = max(y, 0);
      ym = ym * ym / 100;
      if(t < 300)
      {
        w[(t / 4) & 3]->draw(graphics, 0, 0, 0, 0, 160, 144);
        b2[max(3 - max(ym - 20, 0) / 2, 0)]->draw(graphics, 71, 12 - ym, 3);
      }
      else
        w[((t / 4) & 3) + 4]->draw(graphics, 0, 0, 0, 0, min(t * 16, 160), 144);
      
      if(y > 0)   
          f[rand() & 1]->draw(graphics, 71, 12 - ym, 3);
      if(y < 40 && y > -20 && (gameWon / 2 & 1))
        s[(y + 20) / 20]->draw(graphics, 51, 107, 3);
      if(controllers.pressed(0, GameControllers::Y)) gameWon = 2300;
      if(t > 300)
        branding0.draw(graphics, 0, 0, 0);
    }
    else
    {
      int t = min(gameWon - 2150, 800) / 2;
      stars0.draw(graphics, -100 + t / 4, 0 );
      mars0.draw(graphics, 160 - t / 2, 20 + t / 8, 0);
      starman0.draw(graphics, -130 + t, 150 - t / 2, 0);
      
      graphics.setTextColor(40, -1);
      graphics.setCursor(16, 24);
      char d[] = "CONGRATULATIONS!";
      char c[17];
      int i = 0;
      for(;i < min(((t - 100) / 2), 16); i++)
        c[i] = d[i];
      c[i] = 0;
      graphics.print(c);
    }
    /*graphics.setTextColor(20, -1);
    graphics.setCursor(16, 0);
    graphics.print(gameWon); */
    gameWon++;
  }
  else
  if(gameLost > -1)
  {
    playing = false;
    drawGameScreen();
    for(int y = 0; y < 18 && y < gameLost; y++)
      for(int x = 0; x < 10; x++)
        sprites8.draw(graphics, x * 8 + 16, (17 - y) * 8, 13);
    gameLost++;
  }
  else if(dropping > -1)
  {
    if(dropping == 0)
      lines -= field.drop();
    if(lines < 0) lines = 0;
    if(lines == 0) gameWon = 0;
    dropping--;
    drawGameScreen();
  }
  else
  {
    if(!currentBlock.active)
    {
      currentBlock = nextBlock;
      if(currentBlock.test(field, currentBlock.xPos, currentBlock.yPos, currentBlock.orientation))
      {
        currentBlock.place(field);
        gameLost = 0;
      }
    }
    if(currentBlock.active)
    {
      if(timeToStep == 0 || controllers.pressed(0, GameControllers::DOWN, 10))
      {
        if(currentBlock.test(field, currentBlock.xPos, currentBlock.yPos + 1, currentBlock.orientation))
        {
          currentBlock.place(field);
          if(field.hasFull())
            dropping = 40;
          controllers.clear(0, GameControllers::DOWN);
        }
        else
          currentBlock.down();
        timeToStep = stepTicks[level]; 
      }
      timeToStep--;
      if(directionDelay == 0)
      {
        if(controllers.pressed(0, GameControllers::LEFT, 10) && !currentBlock.test(field, currentBlock.xPos - 1, currentBlock.yPos, currentBlock.orientation))
        {
          currentBlock.left();
          directionDelay = directionTicks;
        }
        if(controllers.pressed(0, GameControllers::RIGHT, 10) && !currentBlock.test(field, currentBlock.xPos + 1, currentBlock.yPos, currentBlock.orientation))
        {
          currentBlock.right();
          directionDelay = directionTicks;
        }
      }
      else
        directionDelay--;

      if(controllers.pressed(0, GameControllers::B) && !currentBlock.test(field, currentBlock.xPos, currentBlock.yPos, (currentBlock.orientation - 1) & 3))
        currentBlock.ccw();
      if(controllers.pressed(0, GameControllers::A) && !currentBlock.test(field, currentBlock.xPos, currentBlock.yPos, (currentBlock.orientation + 1) & 3))
        currentBlock.cw();
    }
    drawGameScreen();
    if(currentBlock.active)
      currentBlock.draw(graphics);
  }
  
  graphics.end();
}

void loop()
{
  draw();
}


