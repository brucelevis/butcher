#include "butcher.h"
#include <view/hud_layer.h>
#include <view/loading_scene.h>
#include <view/dungeon_layer.h>
#include <actors/player.h>
#include <dungeon/dungeon_state.h>
#include <utils/profiler.h>
#include <memory>

namespace cc = cocos2d;

namespace butcher {

Butcher::Butcher()
  : _currentScene(nullptr)
  , _hud(nullptr)
  , _dungeonLevel(0)
{
}

Butcher &Butcher::getInstance()
{
  static Butcher* instance;

  if ( instance == nullptr )
  {
    instance = new Butcher();
    instance->init();
  }

  return *instance;
}

void Butcher::init()
{
  _actors.load("actors_data_wire.bin");

  _currentScene = cocos2d::Scene::create();
  _hud = HudLayer::create();

  BUTCHER.goToLevel(BUTCHER.getDungeonLevel() + 1);
}

cocos2d::Scene* Butcher::getCurrentScene() const
{
  return _currentScene;
}

void Butcher::goToLevel(unsigned level)
{
  if ( level <= 0 )
  {
    cc::log("Can't go to level %d!", level);
    return;
  }

  DungeonState* dungeonState = _dungeons.getLevel(level);

  setPlayerPosition(level, dungeonState);

  _dungeonLevel = level;

  DungeonLayer* layer = DungeonLayer::create();
  layer->setState(dungeonState);

  cc::Scene* new_scene = cocos2d::Scene::create();
  new_scene->addChild(layer);
  new_scene->addChild(_hud);

  cc::Director::getInstance()->replaceScene( new_scene );
  _currentScene = new_scene;
}

void Butcher::setPlayerPosition(unsigned level, DungeonState* dungeonState)
{
  auto actors = dungeonState->getActors([&](std::shared_ptr<Actor> a){
    return a->id() == (level > _dungeonLevel ? Actor::STAIRS_UP : Actor::STAIRS_DOWN);
  });
  if ( !actors.empty() )
    getPlayer()->setPosition(actors.front()->getPosition());
}

unsigned Butcher::getDungeonLevel() const
{
  return _dungeonLevel;
}

std::shared_ptr<Player> Butcher::getPlayer()
{
  if (_player == nullptr)
  {
    _player.reset( _actors.createActor<Player>(1) );
  }

  if ( _player == nullptr )
    cc::log("Butcher::getPlayer: failed to create player");

  return _player;
}

ActorDatabase& Butcher::actorsDatabase()
{
  return _actors;
}

DungeonState *Butcher::getCurrentDungeon()
{
  return _dungeons.getLevel(_dungeonLevel);
}

void Butcher::nextTurn()
{
  DungeonState* d = getCurrentDungeon();
  if ( d )
    d->nextTurn();
}

}
