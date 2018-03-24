#pragma once
#include "Block.h"
#include "Field.h"

class Game
{
  public:
  enum GameType
  {
    A = 0,
    B = 1
  };

  GameType type;
  int x, y, player, level, high, downScore;
  bool multi;

  Block nextBlock;
  Block currentBlock;
  Field field;
  int gameLost;
  bool gameEnded;
  int timeToStep;
  int lines;
  int score;
  int directionDelay;
  int dropping;
  int linesCompleted;

  Game(GameType type, bool multi, int player, int level, int high, int x, int y)
  {
    this->type = type;
    this->multi = multi;
    this->x = x;
    this->y = y;
    this->player = player;
    this->level = level;
    this->high = high;
    lines = (type == B) ? 25 : 0;
    score = 0;    
    directionDelay = 0;
    dropping = -1;
    timeToStep = 30;
    gameLost = -1;
    downScore = 0;
    gameEnded = false;
    
    currentBlock.init(blocks);
    nextBlock.init(blocks);

    field.start(high);
    nextBlock.getNext();
    currentBlock.active = false;
  }

  void printText(Graphics &graphics)
  {
    if(type == A)
    {
      graphics.setCursor(x + 112, 8);
      graphics.setTextColor(0, -1);
      graphics.print("SCORE\n\n");
      graphics.print(score, 10, 5);
      graphics.print("\n\n\nLEVEL\n");
      graphics.print(level, 10, 4);  
      graphics.print("\n\nLINES\n");
      graphics.print(lines, 10, 4);      
    }
    else
    {
      graphics.setCursor(x + 112, 8);
      graphics.setTextColor(0, -1);
      graphics.print("LEVEL\n");
      graphics.print(level, 10, 3);
      graphics.print("\n\nHIGH\n");
      graphics.print(high, 10, 3);  
      graphics.print("\n\n\n\nLINES\n");
      graphics.print(lines, 10, 4);
    }
  }
  
  void draw(Graphics &graphics)
  {
    screens.draw(graphics, (type == A) ? 4 : 5, x, y);
    printText(graphics);
    if(gameLost < 32)
    {
      field.draw(graphics, blocks, x, y, dropping, gameLost > -1 ? 17 - gameLost : 18);
      if(currentBlock.active)
        currentBlock.draw(graphics, x, y);    
    }
    else
    {
      screens.draw(graphics, 6, x, y);
      if(!multi)
        screens.draw(graphics, 7, x, y);
    }
    nextBlock.draw(graphics, x, y);   
  }
  
  void logic(GameControllers &controllers)
  {    
    const int lineScores[4] = {40, 100, 300, 1200}; 
    const int stepTicks[] = {17, 16, 15, 14, 13, 12, 11, 10, 8, 6};
    const int directionTicks = 4;
    linesCompleted = 0;
    if(gameLost > -1)
    {
      if(gameLost == 100)
        gameEnded = true;
      gameLost++;
    }
    else if(dropping > -1)
    {
      if(dropping == 0)
      {
        int l = field.drop();
        if(type == A)
        {
          if(lines / 10 != (lines + l) / 10 && level < 9)
          {
              sounds.play(audioSystem, 9);
              level++;
          }
          lines += l;
        }
        else
        {
          lines -= l;
          if(lines < 0) 
            lines = 0;
        } 
        score += lineScores[l] * (level + 1);
        linesCompleted = l;
        sounds.play(audioSystem, 8);
      }
      dropping--;
    }
    else
    {
      if(!currentBlock.active)
      {
        currentBlock = nextBlock;
        score += downScore;
        downScore = 0;
        if(currentBlock.test(field, currentBlock.xPos, currentBlock.yPos, currentBlock.orientation))
        {
          currentBlock.place(field);
          gameLost = 0;
          sounds.play(audioSystem, 11);
        }
      }
      if(currentBlock.active)
      {
        if(timeToStep == 0 || controllers.pressed(player, GameControllers::DOWN, 10))
        {
          if(timeToStep == 0)
            downScore = 0;
          else
            downScore++;
          if(currentBlock.test(field, currentBlock.xPos, currentBlock.yPos + 1, currentBlock.orientation))
          {
            currentBlock.place(field);
            int f = field.hasFull();
            if(f)
            {
              dropping = 40;
              if(f == 4) 
                sounds.play(audioSystem, 7);
              else
                sounds.play(audioSystem, 6);
            }
            else
              sounds.play(audioSystem, 5);
            controllers.clear(player, GameControllers::DOWN);
          }
          else
            currentBlock.down();
          timeToStep = stepTicks[level]; 
        }
        timeToStep--;
        if(directionDelay == 0)
        {
          if(controllers.pressed(player, GameControllers::LEFT, 10) && !currentBlock.test(field, currentBlock.xPos - 1, currentBlock.yPos, currentBlock.orientation))
          {
            currentBlock.left();
            directionDelay = directionTicks;
            sounds.play(audioSystem, 3);
          }
          if(controllers.pressed(player, GameControllers::RIGHT, 10) && !currentBlock.test(field, currentBlock.xPos + 1, currentBlock.yPos, currentBlock.orientation))
          {
            currentBlock.right();
            directionDelay = directionTicks;
            sounds.play(audioSystem, 3);
          }
        }
        else
          directionDelay--;
  
        if(controllers.pressed(player, GameControllers::B) && !currentBlock.test(field, currentBlock.xPos, currentBlock.yPos, (currentBlock.orientation - 1) & 3))
        {
          currentBlock.ccw();
          sounds.play(audioSystem, 4);
        }
        if(controllers.pressed(player, GameControllers::A) && !currentBlock.test(field, currentBlock.xPos, currentBlock.yPos, (currentBlock.orientation + 1) & 3))
        {
          currentBlock.cw();
          sounds.play(audioSystem, 4);
        }
      }
    }
  } 
};

