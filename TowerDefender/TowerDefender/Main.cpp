/*
* Copyright (C) Altamish Mahomed - All Rights Reserved
* Unauthorized copying of this file or its contents,
* via any medium, is strictly prohibited,
* regardless of intent of use.
* Proprietary and confidential.
* Written by Altamish Mahomed <gambit318@gmail.com>, May 2018
*/

#include <fstream>

#include "bot.h"
using namespace bot;


void bot::ReadGameDetails()
{
  map_width   = j["gameDetails"]["mapWidth"];
  map_height  = j["gameDetails"]["mapHeight"];
  round       = j["gameDetails"]["round"];
  //AL.WHAT IS THE MAX NUMBER OF ROUNDS?!?!?!
  //maxTurns = ;
  
  cost_defense  = j["gameDetails"]["buildingPrices"]["DEFENSE"];
  cost_attack   = j["gameDetails"]["buildingPrices"]["ATTACK"];
  cost_energy   = j["gameDetails"]["buildingPrices"]["ENERGY"];
}

void bot::ReadPlayerDetails()
{
  int myJsonIndex = 0;
  int opponentJsonIndex = 1;
  const string playerType = j["players"][myJsonIndex]["playerType"].get<string>();
  if (playerType != "A")
  {
    myJsonIndex = 1;
    opponentJsonIndex = 0;
  }

  json jm       = j["players"][myJsonIndex];
  me.energy     = jm["energy"];
  me.health     = jm["health"];
  me.hitsTaken  = jm["hitsTaken"];
  me.score      = jm["score"];

  json jo             = j["players"][opponentJsonIndex];
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
  ReadPlayerDetails();
  ReadMap();

  return true;
}


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


void bot::WriteBestActionToFile()
{
  string str = "";

  if (bestAction.action != NONE)
  {
    str 
    += to_string(bestAction.x) + "," 
    + to_string(bestAction.y) + "," 
    + to_string(bestAction.action);
  }

  ofstream movefile(outputFileName);
  movefile << str;
  movefile.close();
}


int bot::GetPossibleActions()
{
  if (me.energy >= cost_attack)
  {
    possibleActions.push_back(BUILD_ATTACK);
  }

  if (me.energy >= cost_defense)
  {
    possibleActions.push_back(BUILD_DEFENSE);
  }

  if (me.energy >= cost_energy)
  {
    possibleActions.push_back(BUILD_ENERGY);
  }

  return possibleActions.size();
}

void bot::GetActionableRows()
{
  //AL.
  //TODO

  //If my side of row is not full, add to vector
  for (int row = 0; row < map_height; ++row)
  {
    for (int col = 0; col < (map_width/2); ++col)
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


void bot::GetBestAction()
{
  //i.e. Which buildings you have enough energy for.
  if (GetPossibleActions() == 0)
  {
    return;
  }
  
  //Get all rows that you can actually play on.
  GetActionableRows();

  //AL.
  //TODO

  
  //For each playable rows, for each cell, simulate every possible play, for n steps
  //n steps = possibly the length of the map, or rounds remaining (whichever is smaller) OR some other value liek 10 lel
  //keep track of the highest difference in yours vs enemy's score. That is, you will want to know which move maximised the score diff.
  //set the best action and return.

}


int main()
{
  if (InitialiseFromJSON() == false)
  {
    return -1;
  } 

  GetBestAction();

  WriteBestActionToFile();

  //Don't bother cleaning up
  //DeleteField();

  return 0;
}