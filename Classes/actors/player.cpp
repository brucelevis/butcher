#include "player.h"
#include <actors/monster.h>
#include <actors/actions/attack_action.h>
#include <actors/actions/shot_action.h>
#include <actors/object.h>
#include <actors/instances/door.h>
#include <butcher.h>
#include <cmath>
#include "cocos2d.h"
#include <utils/utils.h>
#include <butcher.h>
#include <view/loading_scene.h>
#include <dungeon/dungeon_state.h>

namespace cc = cocos2d;

namespace butcher {

Player::Player(const ActorData *data)
  : Character(data)
  , _quickSwitchWeapon(ActorID::INVALID)
{
  _waypoints.insert(1);
}

std::unique_ptr<Actor> Player::clone(std::unique_ptr<Actor> allocated)
{
  Player* p = dynamic_cast<Player*>(allocated.release());
  if ( p == nullptr )
    p = new Player(nullptr);

  p->_inventory = _inventory;
  p->_craftbook = _craftbook;
  p->_waypoints = _waypoints;
  p->_quickSwitchWeapon = _quickSwitchWeapon;

  return std::move( Character::clone(std::move(std::unique_ptr<Actor>{p})) );
}

void Player::load(const SaveData *data)
{
  if ( data )
  {
    _exp = data->exp();
    _level = data->level();

    _inventory = Inventory();
    auto inv = data->inventory();
    for ( unsigned i = 0; i < inv->Length(); ++i)
    {
      const InventoryData* item_data = inv->Get(i);

      //cc::log("%d %d %d", item_data->amount(), item_data->item_id(), item_data->equipped());
      AmountedItem item;
      item.amount = item_data->amount();
      item.item = BUTCHER.actorsDatabase().createActor<Item>( (ActorID)item_data->item_id() );

      if ( item_data->equipped() )
        _inventory.equip(item);
      else
        _inventory.addItem(item);
    }

    auto cb = data->craftbook();

    _craftbook = CraftBook();
    _craftbook.setFreePoints( cb->free_points());

    auto recs = cb->recipes();
    for (unsigned i = 0; i < recs->Length(); ++i )
    {
      _craftbook.addRecipe( BUTCHER.recipesDatabase().createRecipe( (RecipeID)recs->Get(i) ) );
    }

    _waypoints.clear();
    for ( unsigned wIdx = 0; wIdx < data->waypoints()->Length(); ++wIdx )
      _waypoints.insert(data->waypoints()->Get(wIdx));

    cc::log("XXX %d", data->quick_switch());
    setQuickSwitchWeapon(static_cast<ActorID>(data->quick_switch()));
  }
}

void Player::onCollide(std::shared_ptr<Actor> obstacle)
{
  std::shared_ptr<Monster> mob = std::dynamic_pointer_cast<Monster>(obstacle);
  if ( mob )
  {
    if ( isUsingRangedWeapon() )
      performAction( new ShotAction(Target(mob)) );
    else
      performAction( new AttackAction(Target(mob)) );
  }

  std::shared_ptr<Object> obj = std::dynamic_pointer_cast<Object>(obstacle);
  if ( obj )
  {
    obj->onInterract(shared_from_this());
  }
}

void Player::onKill(std::shared_ptr<Character> killed)
{
  int levelDiff = getLevel() - killed->getLevel();
  float multiplier = std::min(std::max(1.f-(levelDiff-1)/10.f,0.f),1.f);
  int expBonus = static_cast<float>(killed->getExp())*multiplier;

  if ( expBonus > 0 )
  {
    fadeText("+"+toStr(expBonus)+"XP",cc::Color4B(126,192,238,255), 1);
    setExp( getExp() + expBonus );
  }
}

void Player::onHit(std::shared_ptr<Character>)
{
  AmountedItem wpn = getInventory().equipped(ItemSlotType::WEAPON);
  if ( wpn.item && wpn.item->rollBreak() )
  {
    getInventory().unequip(ItemSlotType::WEAPON);
    notify(EventData(EventType::ItemDestroyed, "Weapon cracked!"));
  }
}

void Player::onNextTurn()
{
  Character::onNextTurn();

  for ( auto& i : getInventory().getEquippedItems() )
  {
    AmountedItem ai = i.second;
    if ( ai.item )
      ai.item->onNextTurn();
  }
}

void Player::onDestroy(std::shared_ptr<Actor> killer)
{
  int expLoss = getExp() / 20; // 5% exp lost on death
  setExp(getExp() - expLoss);

  std::string msg = "You died!|"
                    " |"
                    "You have been killed by a(n) " + killer->getName() + " and lost " + toStr(expLoss) + "XP!";

  LoadingScreen::run([this, msg](){
    BUTCHER.goToLevel(getHighestWaypoint(), ActorID::WAYPOINT);
    this->notify(EventData(EventType::PlayerDied, msg));
  }, "Respawning..");
}

Inventory& Player::getInventory()
{
  return _inventory;
}

Damage Player::getDamage()
{
  AmountedItem i = getInventory().equipped(ItemSlotType::WEAPON);
  if ( i.item )
  {
    return i.item->getDamage();
  }

  return Character::getDamage();
}

int Player::getAttribute(AttributeType type)
{
  int atr = Character::getAttribute(type);
  for ( auto s : ItemSlotType() )
  {
    AmountedItem i = getInventory().equipped(s);
    if ( i.item != nullptr )
    {
      atr += i.item->getAttribute(type);
    }
  }

  return atr;
}

int Player::takeDamage(Damage damage, std::shared_ptr<Actor> attacker)
{
  int dmg = Character::takeDamage(damage, attacker);

  for ( auto s : ItemSlotType() )
  {
    if ( s != ItemSlotType::WEAPON )
    {
      AmountedItem i = getInventory().equipped(s);
      if ( i.item && i.item->rollBreak() )
      {
        getInventory().unequip(s);
        notify(EventData(EventType::ItemDestroyed, ItemSlotType2Str(s) + " destroyed!"));
      }
    }
  }

  return dmg;
}

void Player::setExp(int exp)
{
  Character::setExp(exp);
  if ( getExp() >= getExpForNextLevel() )
  {
    setExp(getExpForNextLevel() - getExp());
    setLevel( getLevel() + 1 );
    giveLevelUpBonuses();
  }
}

int Player::getExpForNextLevel() const
{
  return getExpForLevel(getLevel() + 1);
}

void Player::giveLevelUpBonuses()
{
  int newCraftPoints = getCraftPointsOnLevel(getLevel());
  getCraftbook().setFreePoints(getCraftbook().getFreePoints() + newCraftPoints);
  setHp( getMaxHp() );
  notify(EventType::LevelUP);
}

bool Player::canShootAt(cocos2d::Vec2 coord)
{
  AmountedItem i = getInventory().equipped(ItemSlotType::WEAPON);

  if ( !isUsingRangedWeapon() )
    return false;

//  //Is in range?
//  int weaponRange = i.item->getRange();
//  float distance = calculateDistance(getTileCoord(), coord);
//  if ( distance > static_cast<float>(weaponRange + 1) )
//    return false;

  //Has ammo?
  ActorID ammoId = i.item->getAmmoId();
  int ammoAmount = getInventory().getItem(ammoId).amount;
  if ( ammoAmount <= 0 )
    return false;

  return true;
}

bool Player::isUsingRangedWeapon()
{
  AmountedItem i = getInventory().equipped(ItemSlotType::WEAPON);

  //Valid item?
  if ( !i.item || i.amount <= 0 )
    return false;

  //Is ranged?
  if ( i.item->getAmmoId() == ActorID::INVALID )
    return false;

  return true;
}

void Player::scheduleAction(std::shared_ptr<ActorAction> action)
{
  _scheduledAction = action;
}

bool Player::hasScheduledAction() const
{
  return _scheduledAction != nullptr;
}

ActorAction::Result Player::triggerScheduledAction(Target target)
{
  if ( hasScheduledAction() )
  {
    _scheduledAction->setTarget(target);
    auto action = _scheduledAction;
    _scheduledAction = nullptr;

    return performAction(action);
  }

  return ActorAction::Result::NOK;
}

bool Player::isOutOfControl()
{
  return getHp() <= 0;
}

bool Player::knowsWaypoint(int level) const
{
  return _waypoints.find(level) != _waypoints.end();
}

void Player::addWaypoint(int level)
{
  _waypoints.insert(level);
}

std::vector<int> Player::getWaypoints() const
{
  std::vector<int> wp;
  for ( int w : _waypoints )
    wp.push_back(w);
  return wp;
}

int Player::getHighestWaypoint() const
{
  return _waypoints.empty() ? 1 : *_waypoints.rbegin();
}

ActorID Player::getQuickSwitchWeaponID() const
{
  return _quickSwitchWeapon;
}

std::shared_ptr<Item> Player::getQuickSwitchWeapon()
{
  return getInventory().getItem(getQuickSwitchWeaponID()).item;
}

void Player::setQuickSwitchWeapon(ActorID itemId)
{
  _quickSwitchWeapon = itemId;
  notify(EventType::QuickSwitchWeaponChanged);
}

ActorAction::Result Player::performAction(std::shared_ptr<ActorAction> action)
{
  ActorAction::Result r = Character::performAction(action);
  if ( r == ActorAction::Result::COST_EXTRA_TURN ){
    BUTCHER.nextTurn(false);
  }
  return r;
}

ActorAction::Result Player::performAction(ActorAction *action)
{
  ActorAction::Result r = Character::performAction(action);
  if ( r == ActorAction::Result::COST_EXTRA_TURN ){
    BUTCHER.nextTurn(false);
  }
  return r;
}

int Player::getExpForLevel(int level) const
{
  return std::pow(level - 1, 3) * 60;
}

CraftBook& Player::getCraftbook()
{
  return _craftbook;
}

}
