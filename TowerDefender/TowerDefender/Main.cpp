#include "nlohmann/json.hpp"
using json = nlohmann::json;

#include "bot.h"
using namespace bot;

#include <fstream>


void bot::WriteMoveToFile(const int x, const int y, const ACTIONS action)
{
  string str = "";

  if (action != NONE)
  {
    str += to_string(x) + "," + to_string(y) + "," + to_string(action);
  }

  ofstream movefile(outputFileName);
  movefile << str;
  movefile.close();
}


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


int main()
{
  if (InitialiseFromJSON() == false)
  {
    return -1;
  }
 
  
  WriteMoveToFile(13, 22, BUILD_ENERGY);

  //Don't bother cleaning up
  //DeleteField();
  return 0;
}