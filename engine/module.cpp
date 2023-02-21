#include "module.h"
#include "physic-module.h"
#include "ai.h"
#include "weapon.h"
#include "entity.h"
#include "map.h"
#include <filesystem>
#include <set>
#include "skill.h"

using namespace ns_engine;
using namespace Json;
using namespace ns_physic_module;
using namespace ns_entity;
using namespace ns_map;
using namespace ns_skill;

namespace ns_module {

MTYPE_MAP_BEG(string, int, kModType)
MTYPE_MAP(building)
MTYPE_MAP(enemy)
MTYPE_MAP(boss)
MTYPE_MAP(npc)
MTYPE_MAP(role)
MTYPE_MAP(tree)
MTYPE_MAP(weapon)
MTYPE_MAP(bullet)
MTYPE_MAP(skill)
MTYPE_MAP(effect)
MTYPE_MAP(combination)
MTYPE_MAP_END


MTYPE_MAP_BEG(string, int, kModDirect)
MTYPE_MAP(idle)
MTYPE_MAP(idle_up)
MTYPE_MAP(idle_down)
MTYPE_MAP(idle_left)
MTYPE_MAP(idle_right)
MTYPE_MAP(up)
MTYPE_MAP(down)
MTYPE_MAP(left)
MTYPE_MAP(right)
MTYPE_MAP_END


MTYPE_MAP_BEG(string, int, kGenerate)
MTYPE_MAP(common)
MTYPE_MAP(hide_in_other)
MTYPE_MAP(from_screen_else)
MTYPE_MAP_END


MTYPE_MAP_BEG(string, Actor::CreateBodyFunc, kCreateBody)
MTYPE_MAP(NoCollFunc)
MTYPE_MAP(CollFunc)
MTYPE_MAP(CollNoRestiFunc)
MTYPE_MAP_END


MTYPE_MAP_BEG(string, Module::CreateAi, kAiMap)
MTYPE_MAP(AiMove)
MTYPE_MAP(AiRandomMoveOnce)
MTYPE_MAP(AiFollow)
MTYPE_MAP(AiCircle)
MTYPE_MAP(AiCircleRole)
MTYPE_MAP(AiMoveAndDie200)
MTYPE_MAP(AiCircleRoleOnly)
MTYPE_MAP(AiLook)
MTYPE_MAP(AiLookAndMove)
MTYPE_MAP(AiLine)
MTYPE_MAP(AiGeneratLine)
MTYPE_MAP(AiCounterclockwise)
MTYPE_MAP_END


MTYPE_MAP_BEG(string, Module::CreateMultAi, kMAiMap)
MTYPE_MAP(MAiMultCircleRole)
MTYPE_MAP(MAiMultCircleRoleOnly)
MTYPE_MAP_END


MTYPE_MAP_BEG(string, Module::CreateWeapon, kWeaponMap)
MTYPE_MAP(WPAlpha)
MTYPE_MAP(WPFollowBullet)
MTYPE_MAP_END


map<string, Ai *> kExAiMap;
map<string, Skill *> kSkillMap;


const string kstrAi = "ai";
const string kstrMAi = "mai";
const string kstrWeapon = "weapon";
const string kstrGoalType = "goaltype";
const string kstrType = "type";
void Module::SetParam(const Value &jsn) {
	auto w = jsn["w"].asInt();
	auto h = jsn["h"].asInt();
	auto collw = jsn["collw"].asInt();
	auto collh = jsn["collh"].asInt();

	auto bodyFunc = jsn.isMember("bodyfunc") ? jsn["bodyfunc"].asString() : "NoCollFunc";
	auto generate = jsn.isMember("generate_type") ? jsn["generate_type"].asString() : "common";
	auto center = jsn.isMember("center") ? jsn["center"].asBool() : true;
	auto autodie = JSON_VAL(jsn, "autodie", Double, 1.);
	/*auto ai = jsn.isMember("ai") ? jsn["ai"].asString() : "";
	auto WP = jsn.isMember("weapon") ? jsn["weapon"].asString() : "";*/

	if (jsn.isMember(kstrAi) && jsn[kstrAi].isArray()) {
		for (auto &it : jsn[kstrAi]) {
			ai_names_.push_back(it.asString());
		}
	}

	if (jsn.isMember(kstrMAi) && jsn[kstrMAi].isArray()) {
		for (auto &it : jsn[kstrMAi]) {
			mai_names_.push_back(it.asString());
		}
	}

	if (jsn.isMember(kstrWeapon) && jsn[kstrWeapon].isArray()) {
		for (auto &it : jsn[kstrWeapon]) {
			wp_names_.push_back(it.asString());
		}
	}

	if (jsn.isMember(kstrGoalType) && jsn[kstrGoalType].isArray()) {
		for (auto &it : jsn[kstrGoalType]) {
			goaltype_.set((mod_type)kModType[it.asString()]);
		}
	}

	if (jsn.isMember(kstrType) && jsn[kstrType].isArray()) {
		for (auto &it : jsn[kstrType]) {
			type_.set((mod_type)kModType[it.asString()]);
		}
	}

	is_center_ = center;
	SetSize(w, h);
	gtype_ = (generate_type)kGenerate[generate];
	SetCreateBodyFunc(kCreateBody[bodyFunc]);
	autodie_ = autodie;

	for (auto &ani : jsn["animat"]) {
		auto stat = ani["state"].asString();
		auto pic = ani["pic"].asString();
		auto dt = ani["dt"].asInt();

		AddAssetAnimation(pic, dt, kModDirect[stat]);
	}
}

void ModuleFactory::LoadAi(const string& strJsn) {
	Json::Value jsn;
	if (!ParseJson(strJsn, jsn)) {
		return;
	}
	auto name = jsn["ainame"].asString();
	auto func = jsn["createfunc"].asString();
	if (func == "ExAiCreateLine") {
		LineData ld;
		ld.loop = jsn["loop"].asBool();
		ld.fixed = jsn["fixedangle"].asBool();
		ld.directx = jsn["directx"].asDouble();
		ld.directy = jsn["directy"].asDouble();
		ld.angle = jsn["angle"].asDouble();
		auto w = Game::Instance()->w_;
		auto h = Game::Instance()->h_;
		for (auto &it : jsn["seq"]) {
			ld.que.push(d_vel(it["x"].asDouble() * w, it["y"].asDouble() * h));
		}
		kExAiMap[name] = ExAiCreateLine(ld);
	}
}

void ModuleFactory::LoadSkill(const string &strJsn) {
	Json::Value jsn;
	if (!ParseJson(strJsn, jsn)) {
		return;
	}
	auto name = jsn["skillname"].asString();
	auto shot = jsn["shot"].asString();
	auto cd = jsn["cd"].asInt();
	auto key = jsn["key"].asString();

	auto skill = new Skill(shot, cd, (SDL_KeyCode)key[0]);
	
	kSkillMap[name] = skill;
}


void ModuleFactory::LoadModules(const string &path) {
	std::filesystem::path src_dir(path); 
	set<string> dir_set;
	for (filesystem::recursive_directory_iterator end, ite(src_dir); ite != end; ++ite) {
		if (!is_directory(ite->path()))
			dir_set.insert(ite->path().string());
	}

	for (auto &it : dir_set) {
		if (it.find("\\module\\combination\\") != string::npos) {
			LoadCombination<CombinationInstance>(ReadFile(it));
		} else if (it.find("\\module\\exai\\") != string::npos) {
			LoadAi(ReadFile(it));
		} else if (it.find("\\module\\skill\\") != string::npos) {
			LoadSkill(ReadFile(it));
		} else {
			LoadModule<ModuleInstance>(ReadFile(it));
		}
	}
}

void ModuleFactory::UnLoadModules() {
	for (auto &it : mods) {
		SAFE_DELETE(it.second);
	}
	mods.clear();

	for (auto &it : combis) {
		SAFE_DELETE(it.second);
	}
	combis.clear();
}

void ModuleFactory::AddToLayer(Combination* mod, Layer* layer, const int& x, const int& y) {
	layer->AddSub(mod);
	mod->SetPostion(x, y);
}


void ModuleFactory::AddToLayer(Module* mod, Layer *layer, const int &x, const int &y, const string &name) {
	layer->AddSub(mod);
	if (!name.empty()) {
		layer->AddToQuickMap(name, mod);
	}

	mod->SetPostion(x, y, mod->is_center_);
	if (layer->world_) {
		layer->world_->RelateWorld(mod);
	}
}

void ModuleFactory::DestroyModule(Layer *layer, Actor *mod) {
	delete mod;
	layer->DeleteFromQuickMap(mod);
}

void Module::Init() {
	for (auto &it : anims_) {//动画所属对象应该更新
		it.second.UpdateParent(this);
	}
}

};
