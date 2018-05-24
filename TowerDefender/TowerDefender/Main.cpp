/*
* Copyright (C) Altamish Mahomed - All Rights Reserved
* Unauthorized copying of this file or its contents,
* via any medium, is strictly prohibited,
* regardless of intent of use.
* Proprietary and confidential.
* Written by Altamish Mahomed <gambit318@gmail.com>, May 2018
*/

#include <random>
#include <fstream>

#ifdef DEBUG
#include <iostream>
#endif

#include "bot.h"
using namespace bot;

/////////////////
//JSON READERS//
///////////////

void bot::ReadGameDetails()
{
  json jg       = j["gameDetails"];

  map_width     = jg["mapWidth"];
  map_height    = jg["mapHeight"];
  round         = jg["round"];
  energyPerTurn = jg["roundIncomeEnergy"];

  kRowByteSize = map_width * sizeof(CELL);
  
  kHalfMapWidth = (map_width / 2);

  //AL.
  //TODO
  //WHAT IS THIS VALUE ?!?! 
  //maxTurns =  

}

void bot::ReadBuildingStats()
{
  json jb                       = j["gameDetails"]["buildingsStats"];

  cost_defense                  = jb["DEFENSE"]["price"];
  health_defense                = jb["DEFENSE"]["health"];
  constructionTime_defense      = jb["DEFENSE"]["constructionTime"];
  constructionScore_defense     = jb["DEFENSE"]["constructionScore"];
  destroyMultiplier_defense     = jb["DEFENSE"]["destroyMultiplier"];

  cost_attack                   = jb["ATTACK"]["price"];
  health_attack                 = jb["ATTACK"]["health"];
  constructionTime_attack       = jb["ATTACK"]["constructionTime"];
  constructionScore_attack      = jb["ATTACK"]["constructionScore"]; 
  damage_attack                 = jb["ATTACK"]["weaponDamage"];
  speed_attack                  = jb["ATTACK"]["weaponSpeed"];
  cooldown_attack               = jb["ATTACK"]["weaponCooldownPeriod"];
  destroyMultiplier_attack      = jb["ATTACK"]["destroyMultiplier"];

  cost_energy                   = jb["ENERGY"]["price"];
  health_energy                 = jb["ENERGY"]["health"];
  constructionTime_energy       = jb["ENERGY"]["constructionTime"];
  constructionScore_energy      = jb["ENERGY"]["constructionScore"];
  energyGeneratedPerTurn_energy = jb["ENERGY"]["energyGeneratedPerTurn"];
  destroyMultiplier_energy      = jb["ENERGY"]["destroyMultiplier"];
}

void bot::ReadPlayerDetails()
{
  {
    /*
    int myJsonIndex = 0;
    int opponentJsonIndex = 1;
    const string playerType = j["players"][myJsonIndex]["playerType"].get<string>();
    if (playerType != "A")
    {
      myJsonIndex = 1;
      opponentJsonIndex = 0;
    }
    */
  }

  json jm             = j["players"][0];//myJsonIndex];
  me.energy           = jm["energy"];
  me.health           = jm["health"];
  me.hitsTaken        = jm["hitsTaken"];
  me.score            = jm["score"];

  json jo             = j["players"][1];//opponentJsonIndex];
  opponent.energy     = jo["energy"];
  opponent.health     = jo["health"];
  opponent.hitsTaken  = jo["hitsTaken"];
  opponent.score      = jo["score"];
}

void bot::ReadMap()
{
  /*
  field_original = new CELL*[map_height];
  for (int i = 0; i < map_height; ++i)
  {
    field_original[i] = new CELL[map_width];
  }
  */

  for (int row = 0; row < map_height; ++row)
  {
    for (int col = 0; col < map_width; ++col)
    {
      /*
      json jg                             = j["gameMap"][row][col];
      field_original[row][col].x          = jg["x"];
      field_original[row][col].y          = jg["y"];
      field_original[row][col].cellOwner  = jg["cellOwner"].get<string>();
      */

      const size_t buildingCount = j["gameMap"][row][col]["buildings"].size();

      if (buildingCount == 0)
      {
        if (col < kHalfMapWidth)
        {
          XY xy;
          xy.x = col;
          xy.y = row;
          actionableCells.push_back(xy);
        }
      }
      else
      {
        for (int buildingIndex = 0; buildingIndex < buildingCount; ++buildingIndex)
        {
          json jb                   = j["gameMap"][row][col]["buildings"][0];

          BUILDING b;
          b.health                  = jb["health"];
          b.constructionTimeLeft    = jb["constructionTimeLeft"];
          b.price                   = jb["price"];
          b.weaponDamage            = jb["weaponDamage"];
          b.weaponSpeed             = jb["weaponSpeed"];
          b.weaponCooldownTimeLeft  = jb["weaponCooldownTimeLeft"];
          b.weaponCooldownPeriod    = jb["weaponCooldownPeriod"];
          b.destroyMultiplier       = jb["destroyMultiplier"];
          b.constructionScore       = jb["constructionScore"];
          b.energyGeneratedPerTurn  = jb["energyGeneratedPerTurn"];
          b.buildingType            = jb["buildingType"].get<string>();
          b.x                       = jb["x"];
          b.y                       = jb["y"];
          b.buildingOwner           = jb["playerType"].get<string>();

          //field_original[row][col].buildings.push_back(b);

          allBuildings.push_back(b);
        }
      }

      const size_t missileCount = j["gameMap"][row][col]["missiles"].size();
      for (int missileIndex = 0; missileIndex < missileCount; ++missileIndex)
      {
        json jm         = j["gameMap"][row][col]["missiles"][missileIndex];

        MISSILE m;
        m.damage        = jm["damage"];
        m.speed         = jm["speed"];
        m.x             = jm["x"];
        m.y             = jm["y"];
        m.missileOwner  = jm["playerType"].get<string>();

        //field_original[row][col].missiles.push_back(m);

        allMissiles.push_back(m);
      }

    }
  }

}

bool bot::InitialiseFromJSON()
{
  ifstream stateStream(kStateFileName);
  if (!stateStream)
  {
    return false;
  }

  j = json::parse(stateStream);
  if (j == nullptr)
  {
    return false;
  }

  ReadGameDetails();
  ReadBuildingStats();
  ReadPlayerDetails();
  ReadMap();

  return true;
}


///////////////
//GAME LOGIC//
/////////////
//AL.
//TODO
void bot::SelectBestActionFromAllActions()
{
  ACTION currentAction = allResultingActions.front();

  for (const ACTION newAction : allResultingActions)
  {

    //Both options have same death scenarios.
    if  (
        (newAction.resultsInDeath_Me == currentAction.resultsInDeath_Me) &&
        (newAction.resultsInDeath_Opponent == currentAction.resultsInDeath_Opponent)
        )
    {
      //Take the action with best scorediff.
      if (currentAction.scoreDiff > newAction.scoreDiff)
      {
        currentAction = newAction;
      }
    }
  
    //Neither player dies.
    else if (
            (newAction.resultsInDeath_Me == false) &&
            (newAction.resultsInDeath_Opponent == false)
            )
    {
      //Take the action with best scorediff.
      if (newAction.scoreDiff > currentAction.scoreDiff)
      {
        currentAction = newAction;
      }
    }

    //Only opponent dies. 
    else if (
            (newAction.resultsInDeath_Opponent == true) &&
            (newAction.resultsInDeath_Me == false)
            )
    {
      //Neither of us would have died, or only I would have died. 
      if  (
          ((currentAction.resultsInDeath_Me == false) && (currentAction.resultsInDeath_Opponent == false)) ||
          (currentAction.resultsInDeath_Me == true)
          )
      {
        currentAction = newAction;
      }
    }
    
    //Only I die, or we both die.     
    else
    {
      //Ignore this action. It's shit. 
      //Self preservation FTW.
    }

  }

  bestAction = currentAction;
}

void bot::AwardEnergy()
{
  int energyBuildingCount_Me = 0;
  int energyBuildingCount_Opponent = 0;

  //tally energy buildings
  for (const BUILDING b : allBuildings)
  {
    if (b.x < kHalfMapWidth)
    {
      ++energyBuildingCount_Me;
    }
    else
    {
      ++energyBuildingCount_Opponent;
    }
  }

  const int energyGenerated_Me = energyGeneratedPerTurn_energy * energyBuildingCount_Me;
  const int energyGenerated_Opponent = energyGeneratedPerTurn_energy * energyBuildingCount_Opponent;

  tempEnergy_Me       += (energyPerTurn + energyGenerated_Me);
  tempEnergy_Opponent += (energyPerTurn + energyGenerated_Opponent);
}

void bot::ReduceConstructionTimeLeft()
{
  for (int i = 0; i < allBuildings.size(); ++i)
  {
    --allBuildings[i].constructionTimeLeft;
  }
}

void bot::ProcessHits(ACTION& action)
{

  //remove missiles that hit a base and reduce player health.
  for (int im = 0; im < allMissiles.size(); ++im)
  {
    MISSILE& m = allMissiles[im];

    if (m.x < 0)
    {
      me.health -= m.damage;
      tempScore_Opponent += (m.damage * 100);
    }
    else if (m.x >= map_width)
    {
      opponent.health -= m.damage;
      tempScore_Me += (m.damage * 100);
    }

    allMissiles.erase(allMissiles.begin() + im);
  }

  //flag any resulting deaths.
  if (me.health <= 0)
  {
    action.resultsInDeath_Me = true;
  }
  if (opponent.health <= 0)
  {
    action.resultsInDeath_Opponent = true;
  }

  //for each building, see if each missile collides.
  for (int i_build = 0; i_build < allBuildings.size(); ++i_build)
  {
    BUILDING& b = allBuildings[i_build];

    //buildings under construction cannot be hit.
    if  (
        (b.buildingType[0] == 'a') ||
        (b.buildingType[0] == 'd') ||
        (b.buildingType[0] == 'e')
        )
    {
      continue;
    }

    for (int i_miss = 0; i_miss < allMissiles.size(); ++i_miss)
    {
      MISSILE& m = allMissiles[i_miss];

      //if a missile collides, set the building's health and remove the missile.
      if ( (b.x == m.x) && (b.y == m.y) && (b.buildingOwner != m.missileOwner))
      {
        b.health -= m.damage;

        //adjust temp scores
        //if (b.buildingOwner == "A")
        if (b.x < kHalfMapWidth)
        {
          tempScore_Opponent += m.damage;
        }
        else
        {
          tempScore_Me += m.damage;
        }

        //remove building if it's completely destroyed. 
        if (b.health <= 0)
        {
          allBuildings.erase(allBuildings.begin() + i_build);
        }

        //remove the missile.
        allMissiles.erase(allMissiles.begin() + i_miss);

        //only one missile can hit a building per turn.
        break;
      }
    }
  }

}

void bot::MoveMissiles()
{
  for (int i = 0; i < allMissiles.size(); ++i)
  {
    MISSILE& m = allMissiles[i];

    if (m.missileOwner == "A")
    {
      m.x += (1 * m.speed);
    }
    else
    {
      m.x += (-1 * m.speed);
    }
  }
}

void bot::SpawnMissiles()
{
  for (int i = 0; i < allBuildings.size(); ++i)
  {
    BUILDING& b = allBuildings[i];

    if (b.buildingType[0] == 'A')
    {
      if (b.weaponCooldownTimeLeft == 0)
      {
        MISSILE m;
        m.damage = damage_attack;
        m.speed = speed_attack;
        m.x = b.x;
        m.y = b.y;
        m.missileOwner = b.buildingOwner;
        allMissiles.push_back(m);
        b.weaponCooldownTimeLeft = b.weaponCooldownPeriod;
      }
      else
      {
        --b.weaponCooldownTimeLeft;
      }
    }
  }
}

void bot::ConstructBuildings()
{

  for (int i = 0; i < allBuildings.size(); ++i)
  {
    BUILDING b = allBuildings[i];

    if (b.constructionTimeLeft == 0)
    {
      //award score for construciton
      if (b.x < kHalfMapWidth)
      {
        tempScore_Me += b.constructionScore;
      }
      else
      {
        tempScore_Opponent += b.constructionScore;
      }

      //actually do the construction
      if (b.buildingType[0] == 'a')
      {
        b.buildingType[0] = 'A';
      }
      else if (b.buildingType[0] == 'd')
      {
        b.buildingType[0] = 'D';
      }
      else if (b.buildingType[0] == 'e')
      {
        b.buildingType[0] = 'E';
      }

    }

  }
}

void bot::RunSteps(const int steps, ACTION& action)
{
  tempScore_Me        = 0;
  tempScore_Opponent  = 0;

  for (int i = 0; i < steps; ++i)
  {
    //If any have zero time remaining to be built. 
    ConstructBuildings();

    //Missiles will be generated from any attack buildings if they can fire that turn.
    SpawnMissiles();

    //The missiles will be immediately moved, based on their speed.
    MoveMissiles();

    //Each missile will hit a building if it hit it during the movement phase.
    //Also remove missiles that have left the map and destroyed buildings/missiles.
    //Don't forget to affect the scores. 
    ProcessHits(action);

    //Note, this will influence the building states for the next round.
    ReduceConstructionTimeLeft(); 

    //Energy will be awarded, based on the baseline amount received and the number of energy buildings a player has.
    AwardEnergy();
  }

  action.scoreDiff = (tempScore_Me - tempScore_Opponent);
}

/*
void bot::CreateCopyOfField()
{
  field_copy = new CELL*[map_height];
  for (int i = 0; i < map_height; ++i)
  {
    field_copy[i] = new CELL[map_width];
    memcpy(field_copy[i], field_original[i], kRowByteSize);
  }
}
*/

void bot::PlaceBuilding(ACTION& action)
{
  BUILDING b;
  b.buildingOwner = "A";
  b.x             = action.x;
  b.y             = action.y;

  if (action.buildAction == BUILD_ENERGY)
  {
    b.price                 = cost_energy;
    b.health                = health_energy;
    b.constructionTimeLeft  = constructionTime_energy;
    b.constructionScore     = constructionScore_energy;
    b.destroyMultiplier     = destroyMultiplier_energy;
    b.buildingType          = underConstructionCharacter_energy;
  }

  else if (action.buildAction == BUILD_ATTACK)
  {
    b.price                   = cost_attack;
    b.health                  = health_attack;
    b.weaponCooldownPeriod    = cooldown_attack;
    b.weaponCooldownTimeLeft  = b.weaponCooldownPeriod;
    b.weaponDamage            = damage_attack;
    b.weaponSpeed             = speed_attack;
    b.constructionTimeLeft    = constructionTime_attack;
    b.constructionScore       = constructionScore_attack;
    b.destroyMultiplier       = destroyMultiplier_attack;
    b.buildingType            = underConstructionCharacter_attack;
  }

  else if (action.buildAction == BUILD_DEFENSE)
  {
    b.price                   = cost_energy;
    b.health                  = health_energy;
    b.energyGeneratedPerTurn  = energyGeneratedPerTurn_energy;
    b.constructionTimeLeft    = constructionTime_energy;
    b.constructionScore       = constructionScore_energy;
    b.destroyMultiplier       = destroyMultiplier_energy;
    b.buildingType            = underConstructionCharacter_defense;
  }

  tempEnergy_Me -= b.price;

  allBuildings.push_back(b);
}

int bot::GetBuildingCostFromAction(BUILD_ACTION& ba)
{
  if (ba == WAIT_ATTACK)
  {
    return cost_attack;
  }
  
  if (ba == WAIT_DEFENSE)
  {
    return cost_defense;
  }

  if (ba == WAIT_ENERGY)
  {
    return cost_energy;
  }

  return 0;
}

void bot::SimulateAction(ACTION& action, int steps)
{
  //Create a copy of the field for editing during simulation. 
  //CreateCopyOfField();

  tempEnergy_Me       = me.energy;
  tempEnergy_Opponent = opponent.energy;

  if (action.buildAction < NONE)
  {
    const int actionCost = GetBuildingCostFromAction(action.buildAction);
    while (tempEnergy_Me < actionCost)
    {
      RunSteps(1, action);
      --steps;
    }
    action.buildAction = static_cast<BUILD_ACTION>(action.buildAction + SHIFTER);
  }

  PlaceBuilding(action);

  RunSteps(steps, action);

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

      allResultingActions.push_back(action);
    }
  }
}

void bot::RandomiseActionableCells()
{
  random_device rd;
  mt19937 g(rd());
  shuffle(actionableCells.begin(), actionableCells.end(), g);
}

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

  SelectBestActionFromAllActions();
}

////////////////
//FILE OUTPUT//
//////////////

void bot::WriteBestActionToFile()
{
  string str = "";

  if (bestAction.buildAction > NONE)
  {
    str 
    +=  to_string(bestAction.x) 
    +   "," 
    +   to_string(bestAction.y) 
    +   "," 
    +   to_string(bestAction.buildAction);
  }

  ofstream movefile(kOutputFileName);
  movefile << str;
  movefile.close();
}

//////////
//UTILS//
////////

void bot::PrintField(CELL** myField)
{
#ifdef DEBUG
  for (int row = 0; row < map_height; ++row)
  {
    for (int col = 0; col < map_width ; ++col)
    {
      CELL c = myField[row][col];
      
      string buildingsString = "_";
      for (BUILDING b : c.buildings)
      {
        if (b.buildingType.length()>0)
        {
          buildingsString = b.buildingType[0];
        }
      }

      string missilesString = "";
      for (MISSILE m : c.missiles)
      {
        if (m.missileOwner == "A")
        {
          missilesString += ">";
        }
        else if (m.missileOwner == "B")
        {
          missilesString+="<";
        }
      }

      cout << " " << "[";
      //cout << " " << "x:" << c.x << "y:" << c.y;
      //cout << " " << "row:" << row << "col:" << col;
      cout << " " << buildingsString;
      //cout << " " << missilesString;
      cout << " " << "]";
    }
    cout << endl;
  }
  cout << endl;
#endif
}

void bot::PrintAllMissiles()
{
#ifdef DEBUG
  for (MISSILE m : allMissiles)
  {
    cout << m.missileOwner << "[" << m.x << "," << m.y << "]" << " ";
  }
  cout << endl << endl;
#endif
}

void bot::DeleteField(CELL** myField)
{
  if (myField != nullptr)
  {
    for (int row = 0; row < map_height; ++row)
    {
      delete[] myField[row];
    }
    delete[] myField;
  }
}

/////////
//MAIN//
///////

int main()
{
  if (InitialiseFromJSON() == false)
  {
    return -1;
  }

  SetBestAction();

  WriteBestActionToFile();

  //DeleteField(field);

  return 0;
}