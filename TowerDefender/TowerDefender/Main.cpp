/*
* Copyright (C) Altamish Mahomed - All Rights Reserved
* Unauthorized copying of this file or its contents,
* via any medium, is strictly prohibited,
* regardless of intent of use.
* Proprietary and confidential.
* Written by Altamish Mahomed <gambit318@gmail.com>, May 2018
*/

#include <fstream>
#include <iostream>

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
  
  //AL.WHAT ARE THESE VALUES ?!?! 
  //TODO
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
  field = new CELL*[map_height];
  for (int i = 0; i < map_height; ++i)
  {
    field[i] = new CELL[map_width];
  }

  for (int row = 0; row < map_height; ++row)
  {
    for (int col = 0; col < map_width; ++col)
    {
      json jg                   = j["gameMap"][row][col];
      field[row][col].x         = jg["x"];
      field[row][col].y         = jg["y"];
      field[row][col].cellOwner = jg["cellOwner"].get<string>();


      const int buildingCount = j["gameMap"][row][col]["buildings"].size();
      for (int buildingIndex = 0; buildingIndex  < buildingCount; ++buildingIndex)
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

        field[row][col].buildings.push_back(b);
      }

      
      const int missileCount = j["gameMap"][row][col]["missiles"].size();
      for (int missileIndex = 0; missileIndex < missileCount; ++missileIndex)
      {
        json jm         = j["gameMap"][row][col]["missiles"][missileIndex];

        MISSILE m;
        m.damage        = jm["damage"];
        m.speed         = jm["speed"];
        m.x             = jm["x"];
        m.y             = jm["y"];
        m.missileOwner  = jm["playerType"].get<string>();

        field[row][col].missiles.push_back(m);
      }

    }
  }

}

bool bot::InitialiseFromJSON()
{
  ifstream stateStream(stateFileName);
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

void bot::SetBestActionFromAllActions()
{
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

//AL.
//TODO
int bot::SimulateRow(int row, int col, BUILD_ACTIONS buildAction, int steps)
{
  return 0;
}

//For each playable row, for each cell, simulate every possible action, for n steps.
//n steps = possibly the length of the map, or rounds remaining (whichever is smaller) OR some other value liek 10 lel
//Keep track of the highest difference in yours vs enemy's score. That is, you will want to know which move maximised the score diff.
//Set the best action and return.
void bot::SimulateActionableRows()
{
  int stepsToSimulate = map_width;
  if (maxTurns > 0)
  {
    if ((round + stepsToSimulate) > maxTurns)
    {
      stepsToSimulate = (maxTurns - round);
    }
  }

  for (int row : actionableRows)
  {
    for (int col = 0; col < (map_width / 2); ++col)
    {
      for (BUILD_ACTIONS buildAction : possibleBuildActions)
      {
        const int scoreDiff = SimulateRow(row, col, buildAction, stepsToSimulate);

        ACTION action;
        action.x = col;
        action.y = row;
        action.buildAction = buildAction;
        action.scoreDiff = scoreDiff;

        allActions.push_back(action);
      }
    }
  }
}

void bot::SetActionableRows()
{
  //If my side of row is not full, add to vector
  for (int row = 0; row < map_height; ++row)
  {
    for (int col = 0; col < (map_width / 2); ++col)
    {
      //This cell is free to build on,
      //so add row to list and move onto next row.
      if (field[row][col].buildings.size() == 0)
      {
        actionableRows.push_back(row);
        break;
      }
    }
  }
}

void bot::SetPossibleBuildActions()
{
  if (me.energy >= cost_attack)
  {
    possibleBuildActions.push_back(BUILD_ATTACK);
  }

  if (me.energy >= cost_defense)
  {
    possibleBuildActions.push_back(BUILD_DEFENSE);
  }

  if (me.energy >= cost_energy)
  {
    possibleBuildActions.push_back(BUILD_ENERGY);
  }
}

void bot::SetBestAction()
{
  bestAction.buildAction = NONE;

  //i.e. Which buildings you have enough energy for.
  SetPossibleBuildActions();
  if (possibleBuildActions.size() == 0)
  {
    return;
  }

  //Set all rows that you can actually play on.
  SetActionableRows();
  if (actionableRows.size() == 0)
  {
    return;
  }

  //Run the sim on each actionable row and set a list of actions.
  SimulateActionableRows();

  SetBestActionFromAllActions();
}

////////////////
//FILE OUTPUT//
//////////////

void bot::WriteBestActionToFile()
{
  string str = "";

  if (bestAction.buildAction != NONE)
  {
    str
      += to_string(bestAction.x) + ","
      + to_string(bestAction.y) + ","
      + to_string(bestAction.buildAction);
  }

  ofstream movefile(outputFileName);
  movefile << str;
  movefile.close();
}

/////////////
//CLEANUPS//
///////////

void bot::DeleteField()
{
  if (field != nullptr)
  {
    for (int row = 0; row < map_height; ++row)
    {
      delete[] field[row];
    }
    delete[] field;
  }
}

//////////
//UTILS//
////////

void bot::Print()
{
  for (int row = 0; row < map_height; ++row)
  {
    for (int col = 0; col < map_width ; ++col)
    {
      CELL c = field[row][col];
      
      string buildingsString = "_";
      for (BUILDING b : c.buildings)
      {
        //AL. 
        //WTF BROKEN
        if (b.buildingType.length()>0)
        {
          buildingsString = b.buildingType[0];
        }
      }

      string misslesString = "";
      for (MISSILE m : c.missiles)
      {
        if (m.missileOwner == "A")
        {
          misslesString += ">";
        }
        else if (m.missileOwner == "B")
        {
          misslesString+="<";
        }
      }

      cout << " " << "[";
      //cout << " " << "x:" << c.x << "y:" << c.y;
      //cout << " " << "row:" << row << "col:" << col;
      cout << " " << buildingsString;
      //cout << " " << misslesString;
      cout << " " << "]";
    }
    cout << endl;
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
  //Print();

  SetBestAction();

  WriteBestActionToFile();

  //Don't bother cleaning up
  //DeleteField();

  return 0;
}