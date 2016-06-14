#include "level_manager.h"
#include <dungeon/dungeon_state.h>
#include <dungeon/generators/cave_grid_generator.h>
#include <dungeon/generators/dungeon_maker_generator.h>
#include <levels_generated.h>

namespace cc = cocos2d;

namespace butcher {

LevelManager::LevelManager()
{
  _levels.load("levels_data_wire.bin");
}

DungeonState *LevelManager::getLevel(int level)
{
  auto it = _dungeons.find(level);
  DungeonState* dungeonState = it != _dungeons.end() ? it->second : nullptr;

  if ( dungeonState == nullptr )
  {
    dungeonState = new DungeonState();
    dungeonState->setMap( generateMap(level) );
    _dungeons[level] = dungeonState;
  }

  return dungeonState;
}

cc::TMXTiledMap *LevelManager::generateMap(unsigned level)
{
  const LevelData* levelData = _levels.getLevelData(level);

  if ( !levelData )
  {
    cc::log("LevelManager::generateMap level id=%u not found!", level);
    return nullptr;
  }

  std::unique_ptr<GridGenerator> gen;

  switch(levelData->generator())
  {
    case GeneratorType_NaturalCave:
      gen.reset( new CaveGenerator );
    break;
    case GeneratorType_DungeonMaker:
      gen.reset( new DungeonMakerGenerator );
    break;
  }

  if ( !gen )
  {
    cc::log("LevelManager::generateMap invalid level generator %d!", levelData->generator());
    return nullptr;
  }

  _mapBuilder.setMapTemplate( levelData->map_template()->c_str() );
  return _mapBuilder.build( gen->generate(levelData) );
}

}