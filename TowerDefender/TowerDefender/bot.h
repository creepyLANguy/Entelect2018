/*
* Copyright (C) Altamish Mahomed - All Rights Reserved
* Unauthorized copying of this file or its contents, 
* via any medium, is strictly prohibited, 
* regardless of intent of use. 
* Proprietary and confidential. 
* Written by Altamish Mahomed <gambit318@gmail.com>, May 2018
*/

#ifndef BOT_H
#define BOT_H

#include <string>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

using namespace std;

namespace bot
{  

  ///////////////////////////////////////
  //ENUMS 
  ///////////////////////////////////////
  enum BUILD_ACTIONS
  {
    NONE          = -1,
    BUILD_DEFENSE = 0,
    BUILD_ATTACK  = 1,
    BUILD_ENERGY  = 2,

  };

  ///////////////////////////////////////
  //STRUCTS 
  ///////////////////////////////////////

  struct PLAYER
  {
    int energy    = -1;
    int health    = -1;
    int hitsTaken = -1;
    int score     = -1;
  };

  struct BUILDING
  {
    int health;
    int constructionTimeLeft;
    int price;
    int weaponDamage;
    int weaponSpeed;
    int weaponCooldownTimeLeft;
    int weaponCooldownPeriod;
    int destroyMultiplier;
    int constructionScore;
    int energyGeneratedPerTurn;
    string buildingType;
    int x;
    int y;
    string buildingOwner;
  };

  struct MISSILE
  {
    int damage;
    int speed;
    int x;
    int y;
    string missileOwner;
  };

  struct CELL
  {
    int x = -1;
    int y = -1;
    vector<BUILDING> buildings;
    vector<MISSILE> missiles;
    string cellOwner;
  };

  struct ACTION
  {
    int x = -1;
    int y = -1;
    BUILD_ACTIONS buildAction = NONE;
    int scoreDiff = 0;
  };

  ///////////////////////////////////////
  //CONSTANTS 
  ///////////////////////////////////////

  const int maxRuntime = 2000;
  const string stateFileName = "state.json";
  const string outputFileName = "command.txt";

  ///////////////////////////////////////
  //MEMBERS 
  ///////////////////////////////////////

  int cost_defense                        = 30;
  int health_defense                      = 20;
  int constructionTime_defense            = 3;
  int constructionScore_defense           = 1;
  int destroyMultiplier_defense           = 1;
  char constructedCharacter_defense       = 'D';
  char underConstructionCharacter_defense = 'd';

  int cost_attack                         = 30;
  int health_attack                       = 5;
  int cooldown_attack                     = 3;
  int damage_attack                       = 5;
  int speed_attack                        = 1;
  int constructionTime_attack             = 1;
  int constructionScore_attack            = 1;
  int destroyMultiplier_attack            = 1;
  char constructedCharacter_attack        = 'A';
  char underConstructionCharacter_attack  = 'a';

  int cost_energy                         = 20;
  int health_energy                       = 5;
  int energyGeneratedPerTurn_energy       = 3;
  int constructionTime_energy             = 1;
  int constructionScore_energy            = 1;
  int destroyMultiplier_energy            = 1;
  char constructedCharacter_energy        = 'E';
  char underConstructionCharacter_energy  = 'e';

  int startingEnergy  = 20;
  int energyPerTurn   = 5;

  int map_width       = -1;
  int map_height      = -1;
  int maxTurns        = -1;
  int round           = -1;

  PLAYER me;
  PLAYER opponent;

  CELL** field = nullptr;

  json j = nullptr;

  vector<BUILD_ACTIONS> possibleBuildActions;

  vector<int> actionableRows;

  vector<ACTION> allActions;

  ACTION bestAction;

  ///////////////////////////////////////
  //FUNCTIONS 
  ///////////////////////////////////////

  //JSON READERS
  bool InitialiseFromJSON();
  void ReadGameDetails();
  void ReadBuildingStats();
  void ReadPlayerDetails();
  void ReadMap();

  //FILE OUTPUT
  void WriteBestActionToFile();

  //CLEANUPS
  void DeleteField();  

  //GAME LOGIC
  void SetBestAction();
  void SetPossibleBuildActions();
  void SetActionableRows();
  void SimulateActionableRows();
  int  SimulateRow(int row, int col, BUILD_ACTIONS action, int steps);
  void SetBestActionFromAllActions();
}

#endif // BOT_H
