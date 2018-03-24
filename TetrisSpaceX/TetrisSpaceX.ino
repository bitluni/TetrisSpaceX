//code by bitluni give me a shout-out if you like it
#include "AudioSystem.h"
#include "AudioOutput.h"
#include "Graphics.h"
#include "Image.h"
#include "CompositeOutput.h"
#include "Block.h"
#include "Field.h"
#include "GameControllers.h"
#include "Sprites.h"
#include "Font.h"

//tons of uncleaned code
#include "includes.h"


const int MAX_PLAYERS = 2;
////////////////////////////
//pick your controller types
const int LATCH = 16;
const int CLOCK = 17;
const GameControllers::Type CONTROLLER_TYPES[] = {GameControllers::NES, GameControllers::NES};
const int CONTROLLER_DATA_PINS[] = {18, 19};

////////////////////////////
//audio configuration

const int SAMPLING_RATE = 22050;
const int BUFFER_SIZE = 1000;
AudioSystem audioSystem(SAMPLING_RATE, BUFFER_SIZE);
AudioOutput audioOutput;

//PAL MAX, half: 324x268 full: 648x536
//NTSC MAX, half: 324x224 full: 648x448
const int XRES = 320;
const int YRES = 144;

Graphics graphics(XRES, YRES);
CompositeOutput composite(CompositeOutput::NTSC, XRES * 2, YRES * 2);

Font font(8, 8, tetrisFont::pixels);

#include <soc/rtc.h>

#include "Game.h"

GameControllers controllers;

volatile int frame = 0;
int musicId = -1;
Game *games[MAX_PLAYERS];
int level = 0;
int high = 0;
int playerCount = 1;
int playersActive;
bool bTypeSelect = false;
int musicSelect = 0;
int winTime = 0;
int winStep = 0;
const int musicSelectPos[4][2] = {{24, 96}, {88, 96}, {24, 112}, {88, 112}};
const char *musicSelectName[4] = {"A-TYPE", "B-TYPE", "C-TYPE", " OFF"};
const int musicSelectTrack[4] = {0, -1, -1, -1};

enum GameState
{
  START_SCREEN = 0,
  TYPE_SELECT = 1,
  MUSIC_SELECT = 2,
  LEVEL_SELECT = 3,
  HIGH_SELECT = 4,
  GAME_ON = 5,
  MULTI_GAME_END = 6,
  GAME_LOST = 7,
  WIN = 8
};

int gameState = START_SCREEN;

void compositeCore(void *data)
{
  while (true)
  {
    composite.sendFrameHalfResolution(&graphics.frame);
    frame++;
  }
}

void setup()
{
  rtc_clk_cpu_freq_set(RTC_CPU_FREQ_240M);              //highest cpu frequency
  composite.init();
  graphics.init();
  graphics.setFont(font);
  audioOutput.init(audioSystem);
  xTaskCreatePinnedToCore(compositeCore, "c", 1024, NULL, 1, NULL, 0);
  srand(1337);
  controllers.init(LATCH, CLOCK);
  for(int p = 0; p < MAX_PLAYERS; p++)
  {
    controllers.setController(p, CONTROLLER_TYPES[p], CONTROLLER_DATA_PINS[p]);
  }
}

void stopMusic()
{
  if(musicId > -1)
  {
    music.stop(audioSystem, musicId);
    musicId = -1;
  }
}

void playMusic(int track = -1, bool loop = true)
{
  stopMusic();
  if(track == -1)
    musicId = music.play(audioSystem, musicSelectTrack[musicSelect], 1, 1, loop);
  else
    musicId = music.play(audioSystem, track, 1, 1, loop);
}

void startGame()
{
  playMusic();
  int x = playerCount == 1 ? (graphics.xres - screens.xres(0)) / 2 : 0;
  for(int p = 0; p < playerCount; p++)
    games[p] = new Game(bTypeSelect ? Game::B : Game::A, playerCount > 1, p, level, high, x + screens.xres(0) * p, 0);
  playersActive = playerCount;
  gameState = GAME_ON;
  winTime = 0;
  winStep = 0;
}

void endGame()
{
  for(int p = 0; p < playerCount; p++)
  {
    delete games[p];
  }
  gameState = LEVEL_SELECT;
  playMusic();
  sounds.stop(audioSystem);
}

#include "WinScreens.h"

void loop()
{
  static int lastFrameMillis = 0;
  int thisFrameMillis = millis();
  if(thisFrameMillis - lastFrameMillis < (1000 / 30)) return;
  lastFrameMillis = thisFrameMillis;
  audioSystem.calcSamples();
  
  controllers.poll();
  static int blink = 0;
  blink++; 
  
  graphics.begin(true);  
  switch(gameState)
  {
    case START_SCREEN:
    {
      bool changed = false;
      bool selected = false;
      for(int p = 0; p < MAX_PLAYERS; p++)
      {
        if(controllers.pressed(p, GameControllers::START) | controllers.pressed(p, GameControllers::A))
        {
          sounds.play(audioSystem, 1);
          if(playerCount > 1)
          {
            gameState = MUSIC_SELECT;
            bTypeSelect = false;   
          }
          else
            gameState = TYPE_SELECT;
          playMusic();          
          break;
        }
        else
        {
            if(controllers.pressed(p, GameControllers::LEFT) && playerCount == 2)
            {
              playerCount = 1;
              sounds.play(audioSystem, 0);
            }
            else
            if(controllers.pressed(p, GameControllers::RIGHT) && playerCount == 1)
            {
              playerCount = 2;
              sounds.play(audioSystem, 0);              
            }        
        }
      }
      int x = (graphics.xres - screens.xres(0)) / 2;
      int y = (graphics.yres - screens.yres(0)) / 2;
      screens.draw(graphics, 0, x, y);
      graphics.setCursor(x + (playerCount == 1? 8 : 88), y + 112);
      graphics.setTextColor(0, -1);
      graphics.print("\x7c");
      break;
    }
    case TYPE_SELECT:
    case MUSIC_SELECT:
    {
      bool changed = false;
      bool selected = false;
      bool back = false;
      for(int p = 0; p < MAX_PLAYERS; p++)
      {
        if(controllers.pressed(p, GameControllers::B))
        {
          if(gameState == MUSIC_SELECT && playerCount == 1)
            gameState = TYPE_SELECT;
          else
          {
            stopMusic();
            gameState = START_SCREEN;
          }
          break;
        }
        else if(controllers.pressed(p, GameControllers::START) | controllers.pressed(p, GameControllers::A))
        {
          if(gameState == TYPE_SELECT)
            gameState = MUSIC_SELECT;
          else
          {
            gameState = LEVEL_SELECT;
            playMusic();
          }
          break;
        }
        else 
        {
          if(gameState == TYPE_SELECT)
          {
            if(controllers.pressed(p, GameControllers::LEFT) && bTypeSelect)
            {
              bTypeSelect = false;
              sounds.play(audioSystem, 0);
            }
            else
            if(controllers.pressed(p, GameControllers::RIGHT) && !bTypeSelect)
            {
              bTypeSelect = true;
              sounds.play(audioSystem, 0);              
            }
          }
          else
          {
            if(controllers.pressed(p, GameControllers::LEFT) && (musicSelect & 1) == 1)
            {
              musicSelect--;
              playMusic();
            }
            else
            if(controllers.pressed(p, GameControllers::RIGHT) && (musicSelect & 1) == 0)
            {
              musicSelect++;
              playMusic();              
            }
            else
            if(controllers.pressed(p, GameControllers::UP) && (musicSelect & 2) == 2)
            {
              musicSelect -= 2;
              playMusic();
            }
            else
            if(controllers.pressed(p, GameControllers::DOWN) && (musicSelect & 2) == 0)
            {
              musicSelect += 2;
              playMusic();              
            }            
          }
        }
      }      
      int x = (graphics.xres - screens.xres(1)) / 2;
      int y = (graphics.yres - screens.yres(1)) / 2;
      screens.draw(graphics, 1, x, y);
      graphics.setTextColor(0, -1);
      if((blink & 15) < 8 || gameState == MUSIC_SELECT)
      {
        if(bTypeSelect)
        {
          graphics.setCursor(x + 88, y + 40);
          graphics.print("B-TYPE");
        }
        else
        {
          graphics.setCursor(x + 24, y + 40);
          graphics.print("A-TYPE");        
        }
      }
      if((blink & 15) < 8 || gameState == TYPE_SELECT)
      {
        graphics.setCursor(x + musicSelectPos[musicSelect][0], y + musicSelectPos[musicSelect][1]);
        graphics.print(musicSelectName[musicSelect]);        
      }      
      break;
    }
    case LEVEL_SELECT:
    case HIGH_SELECT:
    {

      for(int p = 0; p < MAX_PLAYERS; p++)
      {
        if(controllers.pressed(p, GameControllers::B))
        {
          if(gameState == HIGH_SELECT)
            gameState = LEVEL_SELECT;
          else
          {
            sounds.play(audioSystem, 0);
            playMusic();
            gameState = MUSIC_SELECT;        
          }
          break;
        }
        else if(controllers.pressed(p, GameControllers::START) | controllers.pressed(p, GameControllers::A))
        {
          if(gameState == LEVEL_SELECT && bTypeSelect)
            gameState = HIGH_SELECT;
          else
            startGame();
          break;
        }
        else 
        {
          if(gameState == HIGH_SELECT)
          {
            if(controllers.pressed(p, GameControllers::LEFT) && high > 0)
              { high--; sounds.play(audioSystem, 0); }
            else
            if(controllers.pressed(p, GameControllers::RIGHT) && high < 5)
              { high++; sounds.play(audioSystem, 0); }
            else
            if(controllers.pressed(p, GameControllers::UP) && high > 2)
              { high -= 3; sounds.play(audioSystem, 0); }
            else
            if(controllers.pressed(p, GameControllers::DOWN) && high < 3)
              { high += 3; sounds.play(audioSystem, 0); }
          }
          else
          {
            if(controllers.pressed(p, GameControllers::LEFT) && level > 0)
              { level--; sounds.play(audioSystem, 0); }
            else
            if(controllers.pressed(p, GameControllers::RIGHT) && level < 9)
              { level++; sounds.play(audioSystem, 0); }
            else
            if(controllers.pressed(p, GameControllers::UP) && level > 4)
              { level -= 5; sounds.play(audioSystem, 0); }
            else
            if(controllers.pressed(p, GameControllers::DOWN) && level < 5)
              { level += 5; sounds.play(audioSystem, 0); }
          }
        }
      }      
      int x = (graphics.xres - screens.xres(1)) / 2;
      int y = (graphics.yres - screens.yres(1)) / 2;
      
      graphics.setTextColor(0, -1);
      if(bTypeSelect)
      {
        screens.draw(graphics, 3, x, y);
        graphics.setCursor(x + 16 + 16 * (level % 5), y + 48 + 16 * (level / 5));
        if((blink & 15) < 8 || gameState == HIGH_SELECT)
          graphics.print(level);
        graphics.setCursor(x + 104 + 16 * (high % 3), y + 48 + 16 * (high / 3));
        if((blink & 15) < 8 || gameState == LEVEL_SELECT)
          graphics.print(high);
      }
      else
      {
        screens.draw(graphics, 2, x, y);
        graphics.setCursor(x + 40 + 16 * (level % 5), y + 48 + 16 * (level / 5));
        if((blink & 15) < 8)
          graphics.print(level);
      }
      break;
    }
    case GAME_ON:
    {
      if(controllers.pressed(0, GameControllers::SELECT)) gameState = WIN;
      for(int p = 0; p < playerCount; p++)
      {
        games[p]->logic(controllers);
        if(games[p]->gameEnded)
          playersActive--;
        if(games[p]->gameLost == 0)// && playerCount == 1)
          stopMusic();
        if(bTypeSelect && games[p]->lines == 0)
        {
          gameState = WIN;
          stopMusic();
          break;
        }
      }
      for(int p = 0; p < playerCount; p++)
        games[p]->draw(graphics);
      for(int p = 0; p < playerCount; p++)
        if(games[p]->linesCompleted > 1)
          for(int i = 0; i < playerCount; i++)
            if(i != p) games[i]->field.addLines(games[p]->linesCompleted - 1);
      if(playerCount > 1 && playersActive == 1)
      {
        gameState = MULTI_GAME_END;
        playMusic(2, false);
      }
      if(playerCount == 1 && playersActive == 0)
      {
        gameState = GAME_LOST;
      }
      break;
    }
    case MULTI_GAME_END:
    case GAME_LOST:
    {
      for(int p = 0; p < playerCount; p++)
        games[p]->draw(graphics);
      for(int p = 0; p < playerCount; p++)
        if(controllers.pressed(p, GameControllers::A))
        {
          endGame();
          break;
        }
      break; 
    }
    case WIN:
    {
      const int winTimes[] = {390, 330, 350, 350, 1000000};
      switch(winStep)
      {
        case 0:
          dance(winTime);
          break;
        case 1:
          start(winTime);
          break;
        case 2:
          landing1(winTime);
          break;
        case 3:
          landing2(winTime);
          break;
        case 4:
          mars(winTime);
      }
      winTime++;
      if(winTime > winTimes[winStep])
      {
        winTime = 0;
        winStep++;
      }
      for(int p = 0; p < playerCount; p++)
        if(controllers.pressed(p, GameControllers::A))
        {
          winTime = 0;
          winStep++;
        }
      if(winStep > 4)
        endGame();
      break;
    }
  }
  graphics.end();
}


