/*
* Copyright (C) Altamish Mahomed - All Rights Reserved
* Unauthorized copying of this file or its contents,
* via any medium, is strictly prohibited,
* regardless of intent of use.
* Proprietary and confidential.
* Written by Altamish Mahomed <gambit318@gmail.com>, May 2018
*/

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

      if (buildingCount == 0)
      {
        if (col < (map_width / 2))
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

          field[row][col].buildings.push_back(b);
        }
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

void bot::Print(CELL** myField)
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
  cout << endl;
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

  //Don't bother cleaning up
  //DeleteField(field);

  return 0;
}