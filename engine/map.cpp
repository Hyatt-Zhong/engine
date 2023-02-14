#include "map.h"
#include "module.h"

using namespace ns_engine;
using namespace ns_module;
namespace ns_map {



TestMap::TestMap() {
	element a = {Game::Instance()->w_ / 2, Game::Instance()->h_ / 4, "role"};
	element b = {Game::Instance()->w_ / 3, Game::Instance()->h_ * 3 / 4, "enemy"};
	que.push(a);
	que.push(b);
}

void TestMap::AddTestMod(const string &name) {
	element a = {Game::Instance()->w_ * 2 / 3, Game::Instance()->h_ / 4, name};
	que.push(a);
}

void TestMap::CreateOrUpdateActor() {
	while (!que.empty()) {
		auto e = que.front();
		ModuleFactory::Instance()->Copy(e.name, layer, e.x, e.y);
		que.pop();
	}
}

};
