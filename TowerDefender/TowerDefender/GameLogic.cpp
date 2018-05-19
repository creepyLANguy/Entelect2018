#include <random>

#include "bot.h"
using namespace bot;


///////////////
//GAME LOGIC//
/////////////

void bot::SetBestActionFromAllActions()
{

  //AL.
  //TODO
  //TAKE resultsInDeath_Me AND resultsInDeath_Opponent INTO ACCOUNT


  ACTION tempBestAction = allActions.front();

  for (const ACTION action : allActions)
  {
    if (tempBestAction.scoreDiff < action.scoreDiff)
    {
      tempBestAction.scoreDiff = action.scoreDiff;
    }
    else if (tempBestAction.scoreDiff == action.scoreDiff)
    {
      //AL.
      //TODO
      //IF THE SCORES ARE THE SAME, WHAT's THE NEXT CRITERIA?
      //Just take latest choice for now.
      tempBestAction.scoreDiff = action.scoreDiff;
    }
  }

  bestAction = tempBestAction;
}

int bot::RunSteps(const int steps)
{
  for (int i = 0; i < steps; ++i)
  {
    //Missiles will be generated from any attack buildings if they can fire that turn.
    //SpawnMissles();

    //The missiles will be immediately moved, based on their speed.
    //MoveMissles();

    //Each missile will hit a building if it hit it during the movement phase.
    //RemoveDestroyedMissles();

    //Destroyed buildings will be removed.
    //RemoveDestroyedBuildings();

    //Scores will be awarded to each player, depending on the round.
    //AwardScores();

    //Energy will be awarded, based on the baseline amount received and the number of energy buildings a player has.
    //AwardEnergy();
  }

  return me.score = opponent.score;
}

void bot::CreateCopyOfField()
{
  fieldCopy = new CELL*[map_height];
  for (int i = 0; i < map_height; ++i)
  {
    fieldCopy[i] = new CELL[map_width];
    memcpy(fieldCopy[i], field[i], kRowByteSize);
  }
}

void bot::SimulateAction(ACTION& action, const int steps)
{
  //Create a copy of the field for editing during simulation. 
  CreateCopyOfField();

  int scoreDiff = 0;

  if (action.buildAction < NONE)
  {
    //AL.
    //TODO
    //scoreDiff += RunSteps(amountOfStepsWeNeedToWaitToBuildSpecificBuildingType);
    //steps -= amountOfStepsWeNeedToWaitToBuildSpecificBuildingType;

    action.buildAction = static_cast<BUILD_ACTION>(action.buildAction + SHIFTER);
  }

  BUILDING b = { 0 };
  if (action.buildAction == BUILD_ENERGY)
  {
    b.buildingType = underConstructionCharacter_energy;
  }
  else if (action.buildAction == BUILD_ATTACK)
  {
    b.buildingType = underConstructionCharacter_attack;
  }
  else if (action.buildAction == BUILD_DEFENSE)
  {
    b.buildingType = underConstructionCharacter_defense;
  }

  fieldCopy[action.y][action.x].buildings.push_back(b);

  scoreDiff += RunSteps(steps);

  action.scoreDiff = scoreDiff;

  //DeleteField(fieldCopy);  
}

//For each playable row, for each cell, simulate every possible action, for n steps.
//n steps = possibly the length of the map, or rounds remaining (whichever is smaller) OR some other value liek 10 lel
//Keep track of the highest difference in yours vs enemy's score. That is, you will want to know which move maximised the score diff.
//Set the best action and return.
void bot::SimulateActionableCells()
{
  int stepsToSimulate = map_width;
  if (maxTurns > 0)
  {
    if ((round + stepsToSimulate) > maxTurns)
    {
      stepsToSimulate = (maxTurns - round);
    }
  }

  for (const XY cell : actionableCells)
  {
    for (BUILD_ACTION buildAction : possibleBuildActions)
    {
      ACTION action;
      action.x = cell.x;
      action.y = cell.y;
      action.buildAction = buildAction;
      SimulateAction(action, stepsToSimulate);

      allActions.push_back(action);
    }
  }
}

void bot::RandomiseActionableCells()
{
  random_device rd;
  mt19937 g(rd());
  shuffle(actionableCells.begin(), actionableCells.end(), g);
}

//void bot::SetActionableCells()
/*{
for (int row = 0; row < map_height; ++row)
{
for (int col = 0; col < (map_width / 2); ++col)
{
//This cell is free to build on,
//so add these co-ordinates to list.
if (field[row][col].buildings.size() == 0)
{
XY xy;
xy.x = col;
xy.y = row;
actionableCells.push_back(xy);
}
}
}
}
*/

void bot::SetPossibleBuildActions()
{
  if (me.energy < cost_attack)
  {
    possibleBuildActions.push_back(WAIT_ATTACK);
  }
  else
  {
    possibleBuildActions.push_back(BUILD_ATTACK);
  }

  if (me.energy < cost_defense)
  {
    possibleBuildActions.push_back(WAIT_DEFENSE);
  }
  else
  {
    possibleBuildActions.push_back(BUILD_DEFENSE);
  }

  if (me.energy < cost_energy)
  {
    possibleBuildActions.push_back(WAIT_ENERGY);
  }
  else
  {
    possibleBuildActions.push_back(BUILD_ENERGY);
  }
}

void bot::SetBestAction()
{
  //Set all rows that you can actually play on.
  //SetActionableCells();
  if (actionableCells.size() == 0)
  {
    return;
  }

  bestAction.buildAction = NONE;

  //i.e. Which buildings you have enough energy for.
  SetPossibleBuildActions();
  if (possibleBuildActions.size() == 0)
  {
    return;
  }

  //Randomise order of actionable rows so bot isn't too predictable 
  //Helps if cannot simulate all rows and bot stops at same point each turn.
  RandomiseActionableCells();

  //Run the sim on each actionable row and set a list of actions.
  SimulateActionableCells();

  SetBestActionFromAllActions();
}