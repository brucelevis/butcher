#ifndef MONSTER_H
#define MONSTER_H

#include <actors/character.h>
#include <actors/ai/ai.h>

namespace butcher {

struct ActorData;

class Monster : public Character
{
public:
  Monster(const ActorData* data);

  virtual Actor* clone(Actor* allocated = nullptr);

  virtual void nextTurn();

private:
  Ai _ai;

};

}


#endif // MONSTER_H
