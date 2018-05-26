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

#include "nlohmann/json.hpp"
using json = nlohmann::json;

using namespace std;

namespace bot
{  

  ///////////////////////////////////////
  //ENUMS 
  ///////////////////////////////////////
  enum BUILD_ACTION
  {
    WAIT_DEFENSE  = -4,
    WAIT_ATTACK   = -3,
    WAIT_ENERGY   = -2,
    NONE          = -1,

    BUILD_DEFENSE = 0,
    BUILD_ATTACK  = 1,
    BUILD_ENERGY  = 2,

    SHIFTER       = 4,
  };

  enum ERROR_CODE
  {
    FAIL_JSON_PARSE = -1,
    FAIL_CANT_PLAY  = 0,
    OKAY            = 1,
    TIMEOUT         = 2,
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

    int x                       = -1;
    int y                       = -1;
    string buildingType         = "";
    int health                  = -1;
    int constructionTimeLeft    = -1;
    int price                   = -1;
    int weaponDamage            = -1;
    int weaponSpeed             = -1;
    int weaponCooldownTimeLeft  = -1;
    int weaponCooldownPeriod    = -1;
    int destroyMultiplier       = -1;
    int constructionScore       = -1;
    int energyGeneratedPerTurn  = -1;
    string buildingOwner        = "";
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
    BUILD_ACTION buildAction = NONE;
    vector<int> scoreDiffs;
    bool resultsInDeath_Me = false;
    bool resultsInDeath_Opponent = false;
  };

  struct XY
  {
    int x = -1;
    int y = -1;
  };

  ///////////////////////////////////////
  //CONSTANTS 
  ///////////////////////////////////////

  const int kMaxRuntimeMillis   = 2000;
  const string kStateFileName   = "state.json";
  const string kOutputFileName  = "command.txt";
  
  int kHalfMapWidth = -1; //Yeah okay, we set this later in ReadGameDetails()...

  ///////////////////////////////////////
  //MEMBERS 
  ///////////////////////////////////////

  int cost_defense                          = 30;
  int health_defense                        = 20;
  int constructionTime_defense              = 3;
  int constructionScore_defense             = 1;
  int destroyMultiplier_defense             = 1;
  char* constructedCharacter_defense        = "D";
  char* underConstructionCharacter_defense  = "d";

  int cost_attack                           = 30;
  int health_attack                         = 5;
  int cooldown_attack                       = 3;
  int damage_attack                         = 5;
  int speed_attack                          = 1;
  int constructionTime_attack               = 1;
  int constructionScore_attack              = 1;
  int destroyMultiplier_attack              = 1;
  char* constructedCharacter_attack         = "A";
  char* underConstructionCharacter_attack   = "a";

  int cost_energy                           = 20;
  int health_energy                         = 5;
  int energyGeneratedPerTurn_energy         = 3;
  int constructionTime_energy               = 1;
  int constructionScore_energy              = 1;
  int destroyMultiplier_energy              = 1;
  char* constructedCharacter_energy         = "E";
  char* underConstructionCharacter_energy   = "e";

  int energyPerTurn   = 5;

  int map_width       = -1;
  int map_height      = -1;
  int maxTurns        = -1;
  int round           = -1;

  json j = nullptr;

  vector<BUILDING> allBuildings;
  vector<BUILDING> allBuildings_SimCopy;

  vector<MISSILE> allMissiles;
  vector<MISSILE> allMissiles_SimCopy;

  PLAYER me;
  PLAYER opponent;

  vector<XY> actionableCells_Me;
  vector<XY> actionableCells_Opponent;

  vector<BUILD_ACTION> possibleBuildActions_Me;
  vector<BUILD_ACTION> possibleBuildActions_Opponent;

  vector<ACTION> allResultingActions;

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

  //GAME LOGIC
  ERROR_CODE SetBestAction();
  void SetPossibleBuildActions(PLAYER& player, vector<BUILD_ACTION>& possibleBuildActions);
  void RandomiseActionableCells();
  ERROR_CODE SimulateActionableCells();
  void SimulateAction(ACTION& action_Me, ACTION& action_Opponent, int steps);
  int GetBuildingCostFromAction(BUILD_ACTION& ba);
  int PlaceBuilding(ACTION& action, const char owner);
  void RunSteps(const int steps, ACTION& action, int& tempEnergy_Me, int& tempEnergy_Opponent, int& tempScore_Me, int& tempScore_Opponent);
  void ConstructBuildings(int& tempScore_Me, int& tempScore_Opponent);
  void SpawnMissiles();
  void MoveMissiles();
  void ProcessHits(ACTION& action, int& tempScore_Me, int& tempScore_Opponent);
  void ReduceConstructionTimeLeft();
  void AwardEnergy(int& tempEnergy_Me, int& tempEnergy_Opponent);
  void SelectBestActionFromAllActions();

  //UTILS
#ifdef DEBUG
  void PrintAllMissiles(vector<MISSILE> myMissiles);
#endif

}

#endif // BOT_H
