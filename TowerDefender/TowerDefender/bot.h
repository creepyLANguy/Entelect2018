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
  enum ACTIONS
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
    int energy = 0;
    int health = 0;
    int hitsTaken = 0;
    int score = 0;
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
    int x = 0;
    int y = 0;
    vector<BUILDING> buildings;
    vector<MISSILE> missiles;
    string cellOwner;
  };

  struct BEST_ACTION
  {
    int x = -1;
    int y = -1;
    ACTIONS action = NONE;
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

  int cost_defense = 30;
  int health_defense = 20;
  int constructionTime_defense = 3;
  char constructedCharacter_defense = 'D';
  char underConstructionCharacter_defense = 'd';

  int cost_attack = 30;
  int health_attack = 5;
  int fireRate_attack = 3;
  int damage_attack = 5;
  int constructionTime_attack = 1;
  char constructedCharacter_attack = 'A';
  char underConstructionCharacter_attack = 'a';

  int cost_energy = 20;
  int health_energy = 5;
  int energyGeneratedPerTurn_energy = 3;
  int constructionTime_energy = 1;
  char constructedCharacter_energy = 'E';
  char underConstructionCharacter_energy = 'e';

  int startingEnergy = 20;
  int missileSpeed = 1;
  int missileDamage = 5;
  int energyPerTurn = 5;

  int map_width = 0;
  int map_height = 0;
  int maxTurns = 0;
  int round = 0;

  PLAYER me;
  PLAYER opponent;

  CELL** field = nullptr;

  json j = nullptr;

  vector<ACTIONS> possibleActions;

  BEST_ACTION bestAction;

  ///////////////////////////////////////
  //FUNCTIONS 
  ///////////////////////////////////////

  //JSON READERS
  bool InitialiseFromJSON();
  void ReadGameDetails();
  void ReadPlayerDetails();
  void ReadMap();

  //FILE OUTPUT
  void WriteBestActionToFile();

  //CLEANUPS
  void DeleteField();  

  //GAME LOGIC
  int GetPossibleActions();
  void GetBestAction();
}

#endif // BOT_H
