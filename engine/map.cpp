#include "map.h"
#include "entity.h"
#include "combination.h"
#include <filesystem>
#include <set>
#include "menu.h"

using namespace ns_engine;
using namespace ns_module;
using namespace ns_entity;
using namespace ns_combination;
using namespace std;

namespace ns_map {

TestMap::TestMap() {
	element a = {Game::Instance()->w_ / 2, Game::Instance()->h_ / 4, "role", "leadrole"};
	que.push(a);
	element b = {Game::Instance()->w_ / 5 /*3*/, Game::Instance()->h_ * 3 / 4, "enemy", "enemy"};
	que.push(b);
}

void TestMap::AddTestMod(const string &name, const string &memname, int x /*= 0*/, int y /*= 0*/) {
	if (x == 0 && y == 0) {
		element a = {Game::Instance()->w_ * 2 / 3, Game::Instance()->h_ * 3 / 4, name, memname};
		que.push(a);
	} else {
		element a = {x, y, name, memname};
		que.push(a);
	}
}

void TestMap::CreateOrUpdateActor() {
	auto func = [this](auto &&e) {
		//这里可以添加actor，因为这是在actor列表循环外添加，而actor的的update函数里则不能添加
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
		if (que.empty()) {
			ModuleFactory::Instance()->SafeAddCombinationToLayer<CombinationInstance>(
				"generator", layer, Game::Instance()->w_ * 3 / 4, Game::Instance()->h_ * 3 / 4);
		}
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
			if ((*it)->type_ .test( role)) {
				Game::Instance()->SetLeadrol(nullptr);
			}
			ModuleFactory::Instance()->DestroyModule(layer, *it);
			it = actors.erase(it);
		} else {
			it++;
		}
	}
}

const string kstrComSeq = "comseq";
const string kstrLeadrole = "leadrole";
void CommonMap::Load(const string &strMap) {
	Json::Value jsn;
	if (!ParseJson(strMap, jsn)) {
		return;
	}
	map_name_ = jsn["mapname"].asString();
	bgm_ = jsn["bgm"].asString();
	Game::Instance()->LoadMusic(bgm_);

	if (jsn.isMember(kstrComSeq) && jsn[kstrComSeq].isArray()) {
		for (auto &it : jsn[kstrComSeq]) {
			auto combos = it["combos"];
			queue<ComData> qu;
			for (auto &x : combos) {
				ComData cd;
				cd.x = JSON_VAL(x, "x", Double, 0);
				cd.y = JSON_VAL(x, "y", Double, 0);
				cd.comname = JSON_VAL(x, "combi", String, "");
				qu.push(cd);
			}
			auto needclear = it["needclear"].asBool();
			que_.push(make_pair(qu,needclear));
		}
	}

	auto leadrole = jsn["leadrole"];
	rdata_.x = JSON_VAL(leadrole, "x", Double, 0);
	rdata_.y = JSON_VAL(leadrole, "y", Double, 0);
	rdata_.name = JSON_VAL(leadrole, "name", String, "");
	rdata_.life = JSON_VAL(leadrole, "life", Int, 0);

	if (jsn.isMember("exdata")) {
		auto ex = jsn["exdata"];
		for (auto &it : ex) {
			ExData exdata;
			exdata.x = JSON_VAL(it, "x", Double, 0);
			exdata.y = JSON_VAL(it, "y", Double, 0);
			exdata.name = JSON_VAL(it, "name", String, "");
			exdata_.push_back(exdata);
		}
	}
}
string CommonMap::GetName() {
	return map_name_;
}

void CommonMap::CreateOrUpdateActor() {
	if (!ns_sdl_ast::AssetMgr::Instance()->IsPlayingMus()) {
		Game::Instance()->PlayMusic(bgm_);
	}

	if (rdata_.life > 0 && Game::Instance()->Leadrol() == nullptr) {
		auto actor = ModuleFactory::Instance()->Copy<ModuleInstance>(rdata_.name, layer, Game::Instance()->w_ * rdata_.x,
									     Game::Instance()->h_ * rdata_.y);
		AddToManage(actor);
		Game::Instance()->SetLeadrol(actor);
		if (ce_) {
			ce_(actor);
		}
		rdata_.life--;
	}

	if (!exdata_.empty()) {
		for (auto &it : exdata_) {
			auto x = Game::Instance()->w_ * it.x;
			auto y = Game::Instance()->h_ * it.y;
			x = x == 0 ? 1 : x;
			y = y == 0 ? 1 : y;
			auto actor = ModuleFactory::Instance()->Copy<ModuleInstance>(it.name, layer, x, y);
			AddToManage(actor);
		}
		exdata_.clear();
	}
	auto fn = [](void *mp, void *comb) {
		auto pThis = (CommonMap *)mp;
		pThis->AddCombo((Combination *)comb);
	};
	wait_a_frame_ = false;
	if (cur_combos_.empty() && !que_.empty()) {
		wait_a_frame_ = true;
		auto ele = que_.front();
		que_.pop();
		auto qu = ele.first;
		cur_need_clear = ele.second;
		while (!qu.empty()) {
			auto e = qu.front();
			qu.pop();
			//如果不需要清理，则不用加入到cur_combos_里判断
			if (!cur_need_clear) {
				ModuleFactory::Instance()->SafeAddCombinationToLayer<CombinationInstance>(
					e.comname, layer, Game::Instance()->w_ * e.x, Game::Instance()->h_ * e.y, this);
			} else {
				ModuleFactory::Instance()->SafeAddCombinationToLayer<CombinationInstance>(
					e.comname, layer, Game::Instance()->w_ * e.x, Game::Instance()->h_ * e.y, this, fn);
			}
		}
	}
	if (cur_need_clear) {//需要清理，则等待这波角色的死亡
		for (auto it = cur_combos_.begin(); it != cur_combos_.end();) {
			if ((*it)->IsDeath()) {
				it = cur_combos_.erase(it);
			} else {
				it++;
			}
		}
	} else {
		cur_combos_.clear();
	}
	Clear();
}

bool CommonMap::IsEnd() {
	auto hasEnemy = false;
	for (auto &it : actors) {
		if (it->type_.test(enemy)) {
			hasEnemy = true;
			break;
		}
	}
	if (!hasEnemy && que_.empty() && cur_combos_.empty()&&!wait_a_frame_) {
		return true;
	}
	return false;
}

void MapManager::LoadMaps(const string &path) {
	std::filesystem::path src_dir(path);
	set<string> dir_set;
	for (filesystem::recursive_directory_iterator end, ite(src_dir); ite != end; ++ite) {
		if (!is_directory(ite->path()))
			dir_set.insert(ite->path().string());
	}
	for (auto &it : dir_set) {
		auto map = new MapInstance;
		map->Load(ReadFile(it));
		maps[map->GetName()] = map;
	}
}

void MapManager::UnLoadMaps() {
	for (auto &it : maps) {
		SAFE_DELETE(it.second);
	}
	maps.clear();
}


};
