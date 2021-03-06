#include "actors_database.h"

namespace cc = cocos2d;

namespace butcher {

ActorDatabase::ActorDatabase()
{
}

std::string ActorDatabase::getName(ActorID id)
{
  auto it = _actorTemplates.find(id);
  return it == _actorTemplates.end() ? ""
                                     : it->second->getName();
}

bool ActorDatabase::load(const std::string &fn)
{
  cc::Data file_data = cc::FileUtils::getInstance()->getDataFromFile(fn);

  if ( file_data.getSize() == 0 )
  {
    cc::log("ActorDatabase: failed to load file.");
    return false;
  }

  const ActorsData* data = GetActorsData( file_data.getBytes() );

  if ( data == nullptr )
  {
    cc::log("ActorDatabase: failed to load actors.");
    return false;
  }

  auto actors = data->actors();
  for (unsigned i = 0; i < actors->Length(); ++i)
  {
    const ActorData* d = actors->Get(i);
    _actorTemplates[ static_cast<ActorID>(d->id()) ] = Actor::create(d);
  }

  cc::log("ActorDatabase: loaded %lu actor templates.", _actorTemplates.size());

  return true;
}

std::vector<ActorID> ActorDatabase::getActorIDs(std::function<bool (const Actor &)> filter)
{
  std::vector<ActorID> ids;

  for ( auto& kv : _actorTemplates )
  {
    if ( filter(*kv.second) )
      ids.push_back(kv.first);
  }

  return ids;
}

}
