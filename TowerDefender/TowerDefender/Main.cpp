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
#include <time.h>

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
  maxTurns      = jg["maxRounds"];
  energyPerTurn = jg["roundIncomeEnergy"];
  
  //TODO
  energyScoreMultiplier = 1;
  healthScoreMultiplier = 15;
  //

  kHalfMapWidth = (map_width / 2);
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
  int myJsonIndex = 0;
  int opponentJsonIndex = 1;
  const string playerType = j["players"][myJsonIndex]["playerType"].get<string>();
  if (playerType != "A")
  {
    myJsonIndex = 1;
    opponentJsonIndex = 0;
  }
  
  json jm             = j["players"][myJsonIndex];
  me.energy           = jm["energy"];
  me.health           = jm["health"];
  me.hitsTaken        = jm["hitsTaken"];
  me.score            = jm["score"];

  json jo             = j["players"][opponentJsonIndex];
  opponent.energy     = jo["energy"];
  opponent.health     = jo["health"];
  opponent.hitsTaken  = jo["hitsTaken"];
  opponent.score      = jo["score"];
}

void bot::ReadMap()
{

  for (int row = 0; row < map_height; ++row)
  {
    for (int col = 0; col < map_width; ++col)
    {
      const size_t buildingCount = j["gameMap"][row][col]["buildings"].size();

      if (buildingCount == 0)
      {
        XY xy;
        xy.x = col;
        xy.y = row;

        if (col < kHalfMapWidth)
        {
          actionableCells_Me.push_back(xy);
        }
        else
        {
          actionableCells_Opponent.push_back(xy);
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


/////////////////////
//ACTION SELECTION//
///////////////////

bool bot::SelectBestAction_OnlyOpponentDies()
{
  for (const ACTION runner : allResultingActions)
  {
    if ((runner.deathCount_Me <= 0) && (runner.deathCount_Opponent > 0))
    {
      bestAction = runner;

      for (const ACTION comparator : allResultingActions)
      {
        if ((comparator.deathCount_Me <= 0) && (comparator.deathCount_Opponent > 0))
        {
          if (bestAction.magicNumber < comparator.magicNumber)
          {
            bestAction = comparator;
          }
        }
      }
      return true;
    }
  }

  return false;
}

bool bot::SelectBestAction_NobodyDies()
{
  for (const ACTION runner : allResultingActions)
  {
    if ((runner.deathCount_Me <= 0) && (runner.deathCount_Opponent <= 0))
    {
      bestAction = runner;

      for (const ACTION comparator : allResultingActions)
      {
        if ((comparator.deathCount_Me <= 0) && (comparator.deathCount_Opponent <= 0))
        {
          if (bestAction.magicNumber < comparator.magicNumber)
          {
            bestAction = comparator;
          }
        }
      }
      return true;
    }
  }

  return false;
}

bool bot::SelectBestAction_BothOfUsDie()
{
  for (const ACTION runner : allResultingActions)
  {
    if ((runner.deathCount_Me > 0) && (runner.deathCount_Opponent > 0))
    {
      bestAction = runner;

      for (const ACTION comparator : allResultingActions)
      {
        if ((comparator.deathCount_Me > 0) && (comparator.deathCount_Opponent > 0))
        {
          if (bestAction.magicNumber < comparator.magicNumber)
          {
            bestAction = comparator;
          }
        }
      }
      return true;
    }
  }

  return false;
}

bool bot::SelectBestAction_OnlyIDie()
{
  for (const ACTION runner : allResultingActions)
  {
    if ((runner.deathCount_Me > 0) && (runner.deathCount_Opponent <= 0))
    {
      bestAction = runner;

      for (const ACTION comparator : allResultingActions)
      {
        if ((comparator.deathCount_Me > 0) && (comparator.deathCount_Opponent <= 0))
        {
          if (bestAction.magicNumber < comparator.magicNumber)
          {
            bestAction = comparator;
          }
        }
      }
      return true;
    }
  }

  return false;
}

double bot::GetVariance(ACTION& action, const int averageScoreDiff)
{
  double variance = 0;

  for (int scoreDiff : action.scoreDiffs)
  {
    variance += static_cast<double>((scoreDiff - averageScoreDiff) * (scoreDiff - averageScoreDiff)) / static_cast<double>(action.scoreDiffs.size());
  }

  return variance;
}

void bot::CalculateMagicNumbers()
{
  for (int i = 0; i < allResultingActions.size(); ++i)
  {
    ACTION action = allResultingActions[i];

    const int averageScoreDiff = static_cast<int>(action.scoreDiffsTotal / static_cast<double>(action.scoreDiffs.size()));
    
    double standardDeviation = 1;
    const double variance = GetVariance(action, averageScoreDiff);
    if (variance > 0)
    {
      standardDeviation = sqrt(variance);
    }

    double deathQuotient = 1;
    if ((action.deathCount_Opponent > 0) && (action.deathCount_Me > 0))
    {
      deathQuotient = (action.deathCount_Opponent / action.deathCount_Me);
    }
    
    allResultingActions[i].magicNumber = (averageScoreDiff / standardDeviation) * deathQuotient;
  }
}

void bot::SelectBestActionFromAllActions()
{
  LOG("SelectBestActionFromAllActions()");

  CalculateMagicNumbers();

  //AL.
#ifdef DEBUG
  PrintAllResultingActions();
#endif
  //

  if (SelectBestAction_OnlyOpponentDies() == true)
  {
    return;
  }

  if (SelectBestAction_NobodyDies() == true)
  {
    return;
  }

  if (SelectBestAction_BothOfUsDie() == true)
  {
    return;
  }

  if (SelectBestAction_OnlyIDie() == true)
  {
    return;
  }  
}


///////////////
//GAME LOGIC//
/////////////

DEATH_RESULT bot::GetDeathResult(int& tempHealth_Me, int& tempHealth_Opponent)
{
  LOG("GetDeathResult()");

  if ((tempHealth_Me <= 0) && (tempHealth_Opponent <= 0))
  {
    return BOTH;
  }

  if (tempHealth_Me <= 0)
  {
    return ME;
  }

  if (tempHealth_Opponent <= 0)
  {
    return OPPONENT;
  }

  return NEITHER;  
}

void bot::AwardEnergy(int& tempEnergy_Me, int& tempEnergy_Opponent, int& tempScore_Me, int& tempScore_Opponent)
{
  LOG("AwardEnergy()");

  int energyBuildingCount_Me = 0;
  int energyBuildingCount_Opponent = 0;

  //tally energy buildings
  for (const BUILDING b : allBuildings_SimCopy)
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

  tempScore_Me += energyGenerated_Me;
  tempScore_Opponent += energyGenerated_Opponent;
}

void bot::ReduceConstructionTimeLeft()
{
  LOG("ReduceConstructionTimeLeft()");

  for (int i = 0; i < allBuildings_SimCopy.size(); ++i)
  {
    --allBuildings_SimCopy[i].constructionTimeLeft;
  }
}

void bot::ProcessAllHits(int& tempScore_Me, int& tempScore_Opponent, int& tempHealth_Me, int& tempHealth_Opponent)
{
  LOG("ProcessAllHits()");

  //for each building, see if each missile collides.
  for (int i_build = 0; i_build < allBuildings_SimCopy.size(); ++i_build)
  {
    BUILDING& b = allBuildings_SimCopy[i_build];

    //buildings under construction cannot be hit.
    if  (
        (b.buildingType[0] == 'a') ||
        (b.buildingType[0] == 'd') ||
        (b.buildingType[0] == 'e')
        )
    {
      continue;
    }

    for (int i_miss = 0; i_miss < allMissiles_SimCopy.size(); ++i_miss)
    {
      MISSILE& m = allMissiles_SimCopy[i_miss];

      for (int shifter = (m.speed-1); shifter >= 0; --shifter)
      {
        int x_temp = m.x;
        if (m.missileOwner == "A")
        {
          x_temp -= shifter;
        }
        else
        {
          x_temp += shifter;
        }
        
        //if a missile collides, set the building's health and remove the missile.
        if ((b.x == x_temp) && (b.y == m.y) && (b.buildingOwner != m.missileOwner))
        {
          int damageTaken = m.damage;
          if (b.health < damageTaken)
          {
            damageTaken = b.health;
          }

          b.health -= damageTaken;

          //adjust temp scores
          //if (b.buildingOwner == "A")
          if (b.x < kHalfMapWidth)
          {
            tempScore_Opponent += (damageTaken * b.destroyMultiplier);
          }
          else
          {
            tempScore_Me += (damageTaken * b.destroyMultiplier);
          }

          //remove building if it's completely destroyed. 
          if (b.health <= 0)
          {
            allBuildings_SimCopy.erase(allBuildings_SimCopy.begin() + i_build);
          }

          //remove the missile.
          allMissiles_SimCopy.erase(allMissiles_SimCopy.begin() + i_miss);

          //only one missile can hit a building per turn.
          break;
        }
      }
    }
  }


  //remove missiles that hit a base and reduce player health.
  for (int im = 0; im < allMissiles_SimCopy.size(); ++im)
  {
    MISSILE& m = allMissiles_SimCopy[im];

    int damageTaken = m.damage;

    if (m.x < 0)
    {
      if (tempHealth_Me < damageTaken)
      {
        damageTaken = tempHealth_Me;
      }

      tempHealth_Me -= damageTaken;
      tempScore_Opponent += (damageTaken  * healthScoreMultiplier);

      allMissiles_SimCopy.erase(allMissiles_SimCopy.begin() + im);
    }
    else if (m.x >= map_width)
    {
      if (tempHealth_Opponent < damageTaken)
      {
        damageTaken = tempHealth_Me;
      }

      tempHealth_Opponent -= damageTaken;
      tempScore_Me += (damageTaken * healthScoreMultiplier);

      allMissiles_SimCopy.erase(allMissiles_SimCopy.begin() + im);
    }
  }

}

void bot::MoveMissiles()
{
  LOG("MoveMissiles()");

	//AL.
#ifdef DEBUG
	cout << "BEFORE:\n\n"
	PrintAllMissiles(allMissiles_SimCopy);
#endif
	//

  for (int i = 0; i < allMissiles_SimCopy.size(); ++i)
  {
    MISSILE& m = allMissiles_SimCopy[i];

    if (m.missileOwner == "A")
    {
      m.x += (1 * m.speed);
    }
    else
    {
      m.x += (-1 * m.speed);
    }
  }

  //AL.
#ifdef DEBUG
  cout << "AFTER:\n\n"
  PrintAllMissiles(allMissiles_SimCopy);
#endif
  //
}

void bot::SpawnMissiles()
{
  LOG("SpawnMissiles()");

  for (int i = 0; i < allBuildings_SimCopy.size(); ++i)
  {
    BUILDING& b = allBuildings_SimCopy[i];

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
        allMissiles_SimCopy.push_back(m);

        b.weaponCooldownTimeLeft = b.weaponCooldownPeriod;
      }
      else
      {
        --b.weaponCooldownTimeLeft;
      }
    }
  }
}

void bot::ConstructBuildings(int& tempScore_Me, int& tempScore_Opponent)
{
  LOG("ConstructBuildings()");

  for (int i = 0; i < allBuildings_SimCopy.size(); ++i)
  {
    BUILDING& b = allBuildings_SimCopy[i];

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
        b.weaponCooldownTimeLeft = 0; //So we can spawn the first missile instantly. 
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

int bot::PlaceBuilding(ACTION& action, const char owner)
{
  LOG("PlaceBuilding()");

  BUILDING b;
  b.buildingOwner = owner;
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

  allBuildings_SimCopy.push_back(b);

  return b.price;
}

DEATH_RESULT bot::RunSteps(
  const int steps, 
  ACTION& action_Me, ACTION& action_Opponent, 
  int& tempEnergy_Me, int& tempEnergy_Opponent, 
  int& tempScore_Me, int& tempScore_Opponent, 
  int& tempHealth_Me, int& tempHealth_Opponent)
{
  LOG("RunSteps()");

  DEATH_RESULT res = NEITHER;

  for (int i = 0; i < steps; ++i)
  {

    if (action_Me.buildAction < NONE)
    {
      if(action_Me.associatedBuildCost < tempEnergy_Me)
      {
        action_Me.buildAction = static_cast<BUILD_ACTION>(action_Me.buildAction + SHIFTER);
        tempEnergy_Me -= PlaceBuilding(action_Me, 'A');
      }
    }

    if (action_Opponent.buildAction < NONE)
    {
      if (action_Opponent.associatedBuildCost < tempEnergy_Opponent)
      {
        action_Opponent.buildAction = static_cast<BUILD_ACTION>(action_Opponent.buildAction + SHIFTER);
        tempEnergy_Opponent -= PlaceBuilding(action_Opponent, 'B');
      }
    }


    //If any have zero time remaining to be built. 
    ConstructBuildings(tempScore_Me, tempScore_Opponent);

    //Missiles will be generated from any attack buildings if they can fire that turn.
    SpawnMissiles();

    //The missiles will be immediately moved, based on their speed.
    MoveMissiles();

    //Each missile will hit a building if it hit it during the movement phase.
    //Also remove missiles that have left the map and destroyed buildings/missiles.
    //Don't forget to affect the scores. 
    ProcessAllHits(tempScore_Me, tempScore_Opponent, tempHealth_Me, tempHealth_Opponent);

    //Note, this will influence the building states for the subsequent steps.
    ReduceConstructionTimeLeft(); 

    //Energy will be awarded, based on the baseline amount received and the number of energy buildings a player has.
    AwardEnergy(tempEnergy_Me, tempEnergy_Opponent, tempScore_Me, tempScore_Opponent);

    //flag any resulting deaths.
    res = GetDeathResult(tempHealth_Me, tempHealth_Opponent);
    if (res != NEITHER)
    {
      LOG("EARLY RETURN (DEATH RESULT)");
      return res;
    }
  }

  LOG("return from RunSteps()");
  return res;
}

int bot::GetBuildingCostFromWaitAction(BUILD_ACTION& ba)
{
  LOG("GetBuildingCostFromWaitAction()");

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

void bot::SimulateAction(ACTION& action_Me, ACTION& action_Opponent, const int steps)
{
  allBuildings_SimCopy  = allBuildings;
  allMissiles_SimCopy   = allMissiles;

  int tempEnergy_Me       = me.energy;
  int tempEnergy_Opponent = opponent.energy;

  int tempScore_Me        = 0;
  int tempScore_Opponent  = 0;

  int tempHealth_Me = me.health;
  int tempHealth_Opponent = opponent.health;

  const DEATH_RESULT res = RunSteps(
                                    steps, 
                                    action_Me, action_Opponent, 
                                    tempEnergy_Me, tempEnergy_Opponent, 
                                    tempScore_Me, tempScore_Opponent, 
                                    tempHealth_Me, tempHealth_Opponent
                                    );

  if (res == BOTH)
  {
    ++action_Me.deathCount_Me;
    ++action_Me.deathCount_Opponent;
  }
  else if (res == ME)
  {
    ++action_Me.deathCount_Me;
  }
  else if (res == OPPONENT)
  {
    ++action_Me.deathCount_Opponent;
  }

  const int scoreDiff = tempScore_Me - tempScore_Opponent;

  action_Me.scoreDiffs.push_back(scoreDiff);

  action_Me.scoreDiffsTotal += scoreDiff;
}

//TODO
//Should probably calculate this value more intelligently...
int bot::GetStepsToSimulate()
{
  return map_width * kStepsToSimMultiplier;
}

//For each playable row, for n steps, for each of my actionable cells, 
//simulate every possible action of mine, against every possible action theirs.
//n steps = possibly the length of the map, or rounds remaining (whichever is smaller) OR some other value liek 10 lel
//Keep track of the highest difference in yours vs enemy's score. That is, you will want to know which move maximised the score diff.
//Set the best action and return.
ERROR_CODE bot::SimulateActionableCells()
{
  LOG("SimulateActionableCells()");

  int stepsToSimulate = GetStepsToSimulate();

  if ((round + stepsToSimulate) > maxTurns)
  {
    stepsToSimulate = (maxTurns - round);
  }

  LOG("GOT STEPS TO SIM : " + to_string(stepsToSimulate));

  //TODO
  //Palin suggested executing the sims in parallel for a duration if time,
  //then choose the best action from whatever you've managed to sim in that time. 
  //
  for (const XY cell_Me : actionableCells_Me)
  {
    string myMove = "SIM ALL MOVES FOR MY ACTIONABLE CELL : ";
    myMove += "(";
    myMove += to_string(cell_Me.x);
    myMove += ",";
    myMove += to_string(cell_Me.y);
    myMove += ")";
    LOG(myMove.c_str());

    for (BUILD_ACTION buildAction_Me : possibleBuildActions_Me)
    {
      LOG("SIMING ACTION : " + to_string(buildAction_Me));

      ACTION action_Me;
      action_Me.x                   = cell_Me.x;
      action_Me.y                   = cell_Me.y;
      action_Me.buildAction         = buildAction_Me;
      action_Me.associatedBuildCost = GetBuildingCostFromWaitAction(action_Me.buildAction);

      for (const XY cell_Opponent : actionableCells_Opponent)
      {
        string opMove = "SIM MY BUILDACTION AT CURRENT CELL AGAINST ALL OPPONENT ACTIONS AT CELL : ";
        opMove += "(";
        opMove += to_string(cell_Opponent.x);
        opMove += ",";
        opMove += to_string(cell_Opponent.y);
        opMove += ")";
        LOG(opMove.c_str());

        for (BUILD_ACTION buildAction_Opponent : possibleBuildActions_Opponent)
        {
          LOG("OPPONENT ACTION : " + to_string(buildAction_Opponent));

          ACTION action_Opponent;
          action_Opponent.x                   = cell_Opponent.x;
          action_Opponent.y                   = cell_Opponent.y;
          action_Opponent.buildAction         = buildAction_Opponent;
          action_Opponent.associatedBuildCost = GetBuildingCostFromWaitAction(action_Opponent.buildAction);

          SimulateAction(action_Me, action_Opponent, stepsToSimulate);
          LOG("SINGLE ACTION SIM'D");

          //Reset this as it would have been changed during sim.
          action_Me.buildAction = buildAction_Me;

#ifndef DEBUG
          if ((clock() - startTime) > kMaxSimulationTime)
          {
            LOG("!!!===!!!===TIMEOUT===!!!===!!!");
            return TIMEOUT;
          }
#endif
        }        
      }

      allResultingActions.push_back(action_Me);
      LOG("PUSH MY ACTION AFTER SUCCESSFUL SIM ON THIS CELL");
    }
  }

  LOG("ALL SIMULATED OKAY!");
  return OKAY;
}

void bot::RandomiseActionableCells()
{
  random_device rd;
  mt19937 g(rd());

  shuffle(actionableCells_Me.begin(), actionableCells_Me.end(), g);
  shuffle(actionableCells_Opponent.begin(), actionableCells_Opponent.end(), g);
}

void bot::SetPossibleBuildActions(PLAYER& player, vector<BUILD_ACTION>& possibleBuildActions)
{
  if (player.energy < cost_attack)
  {
    possibleBuildActions.push_back(WAIT_ATTACK);
  }
  else
  {
    possibleBuildActions.push_back(BUILD_ATTACK);
  }

  if (player.energy < cost_defense)
  {
    possibleBuildActions.push_back(WAIT_DEFENSE);
  }
  else
  {
    possibleBuildActions.push_back(BUILD_DEFENSE);
  }

  if (player.energy < cost_energy)
  {
    possibleBuildActions.push_back(WAIT_ENERGY);
  }
  else
  {
    possibleBuildActions.push_back(BUILD_ENERGY);
  }
}

ERROR_CODE bot::SetBestAction()
{
  if (actionableCells_Me.size() == 0)
  {
    LOG("NO ACTIONABLE CELLS");
    return CANT_PLAY;
  }

  bestAction.buildAction = NONE;

  //i.e. Which buildings you have enough energy for.
  SetPossibleBuildActions(me, possibleBuildActions_Me);
  SetPossibleBuildActions(opponent, possibleBuildActions_Opponent);

  if (possibleBuildActions_Me.size() == 0)
  {
    LOG("NO MOVES ACTIONS FOR ME TO TAKE");
    return CANT_PLAY;
  }

  //Randomise order of actionable rows so bot isn't too predictable 
  //Helps if cannot simulate all rows and bot stops at same point each turn.
  RandomiseActionableCells();
  LOG("RANDOMISED BOTH PLAYERS' ACTIONABLE CELLS");

  //Run the sim on each actionable row and set a list of actions.
  const ERROR_CODE er = SimulateActionableCells();
  
  if (allResultingActions.size() > 0)
  {
    SelectBestActionFromAllActions();
  }
  
  return er;
}


////////////////
//FILE OUTPUT//
//////////////

void bot::WriteBestActionToFile()
{
  LOG("WriteBestActionToFile()");

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

  LOG(str);

#ifdef DEBUG
  //AL.
  ofstream movesHistoryFile; 
  movesHistoryFile.open("movesHistory.txt", ios_base::app);
  movesHistoryFile << str;
  movesHistoryFile << '\n';
  movesHistoryFile.close();
#endif
}


//////////
//UTILS//
////////

void bot::PrintAllMissiles(vector<MISSILE> missiles)
{
  for (MISSILE m : missiles)
  {
    cout << m.missileOwner << "[" << m.x << "," << m.y << "]" << " ";
  }
  cout << endl << endl;
}

struct greater_than_key
{
  inline bool operator() (const ACTION& a1, const ACTION& a2) const
  {
    return (a1.magicNumber > a2.magicNumber);
  }
};

void bot::PrintAllResultingActions()
{

  sort(allResultingActions.begin(), allResultingActions.end(), greater_than_key());

  string str = "";
  for (int i = 0; i < allResultingActions.size(); ++i)
  {
    ACTION a = allResultingActions[i];

    sort(a.scoreDiffs.begin(), a.scoreDiffs.end());

    str
      += "#"          + to_string(i) + "\n"
      + "action:"     + to_string(a.buildAction) + " (" + to_string(a.x) + "," + to_string(a.y) + ")" + "\n"
      + "deaths_Op: " + to_string(a.deathCount_Opponent) + "\n"
      + "deaths_Me: " + to_string(a.deathCount_Me) + "\n"
      + "tot: "       + to_string(a.scoreDiffsTotal) + "\n"
      + "avg: "       + to_string(static_cast<int>(a.scoreDiffsTotal / static_cast<double>(a.scoreDiffs.size()))) + "\n"
      + "magic: "     + to_string(a.magicNumber) + "\n\n";

    for (int sd : a.scoreDiffs)
    {
      str += to_string(sd) + "\n";
    }
    str += "\n\n";
  }
  str += "\n\n";

  ofstream movefile("allMoves.txt");
  movefile << str;
  movefile.close();
}

void bot::LOG(string msg)
{
//#ifdef DEBUG
  cout << msg.c_str() << endl;
//#endif
}

/////////
//MAIN//
///////

int main()
{
  startTime = clock();

  LOG("BEGIN");

  if (InitialiseFromJSON() == false)
  {
    LOG("FAILED TO INIT FROM JSON STATE");
    return FAIL_JSON_PARSE;
  }

  const ERROR_CODE er = SetBestAction();

  WriteBestActionToFile();

  LOG("END WITH CODE : " + to_string(er));
  if (er >= OKAY)
  {
    return OKAY;
  }

  return er;
}