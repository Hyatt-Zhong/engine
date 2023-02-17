#include "map.h"
#include "entity.h"

using namespace ns_engine;
using namespace ns_module;
using namespace ns_entity;
namespace ns_map {



TestMap::TestMap() {
	element a = {Game::Instance()->w_ / 2, Game::Instance()->h_ / 4, "role", "leadrole"};
	element b = {Game::Instance()->w_ / 5/*3*/, Game::Instance()->h_ * 3 / 4, "enemy", "enemy"};
	que.push(a);
	que.push(b);
}

void TestMap::AddTestMod(const string &name, const string &modname, int x /*= 0*/, int y /*= 0*/) {
	if (x == 0 && y == 0) {
		element a = {Game::Instance()->w_ * 2 / 3, Game::Instance()->h_ * 3 / 4, name, modname};
		que.push(a);
	} else {
		element a = {x, y, name, modname};
		que.push(a);
	}
}

void TestMap::CreateOrUpdateActor() {
	auto func = [this](auto &&e) {
		auto actor = ModuleFactory::Instance()->Copy<ModuleInstance>(e.modname, layer, e.x, e.y, e.name);
		if (e.name == "leadrole") {
			Game::Instance()->SetLeadrol(actor);
			actor->AddKeyEvent(SDLK_t, [this](const SDL_Keycode &key, const Uint32 &up_or_down, void *data) {
				if (up_or_down == SDL_KEYUP) {
					auto master = (Actor *)data;
					int x, y;
					dynamic_cast<ModuleInstance *>(master)->GetSubGeneratePos(x, y);
					ModuleFactory::Instance()->SafeAddToLayer<ModuleInstance>("FollowBullet", layer, master, x, y, "");
				}
			});
		}
		AddToManage(actor);
	};
	while (!que.empty()) {
		auto e = que.front();
		func(e);
		emts.push_back(e);
		que.pop();
	}

	for (auto &e : emts) {
		if (!layer->GetActor(e.name)) {
			count++;
			if (count >= maxcount) {
				func(e);
				count = 0;
			}
		}
	}

	Clear();
}

void Map::AddToManage(Actor *actor) {
	actors.push_back(actor);
}

void Map::Clear() {
	for (auto it = actors.begin(); it != actors.end();) {
		if ((*it)->IsDestroy()) {
			if ((*it)->type_ == role) {
				Game::Instance()->SetLeadrol(nullptr);
			}
			ModuleFactory::Instance()->DestroyModule(layer, *it);
			it = actors.erase(it);
		} else {
			it++;
		}
	}
}

};
