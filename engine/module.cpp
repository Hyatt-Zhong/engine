#include "module.h"
#include "physic-module.h"
#include "ai.h"
#include "weapon.h"
#include "entity.h"
#include "map.h"
#include <filesystem>
#include <set>

using namespace ns_engine;
using namespace Json;
using namespace ns_physic_module;
using namespace ns_entity;
using namespace ns_map;

namespace ns_module {

MTYPE_MAP_BEG(string, int, kModType)
MTYPE_MAP(building)
MTYPE_MAP(enemy)
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
MTYPE_MAP(SampleFunc)
MTYPE_MAP_END


MTYPE_MAP_BEG(string, Module::CreateAi, kAiMap)
MTYPE_MAP(AiMove)
MTYPE_MAP(AiFollow)
MTYPE_MAP(AiCircle)
MTYPE_MAP(AiCircleRole)
MTYPE_MAP(AiLook)
MTYPE_MAP(AiLookAndMove)
MTYPE_MAP(AiLine)
MTYPE_MAP(AiGeneratLine)
MTYPE_MAP(AiCounterclockwise)
MTYPE_MAP_END


MTYPE_MAP_BEG(string, Module::CreateMultAi, kMAiMap)
MTYPE_MAP(MAiMultCircleRole)
MTYPE_MAP_END


MTYPE_MAP_BEG(string, Module::CreateWeapon, kWeaponMap)
MTYPE_MAP(WPAlpha)
MTYPE_MAP(WPFollowBullet)
MTYPE_MAP_END


const string kstrAi = "ai";
const string kstrMAi = "mai";
const string kstrWeapon = "weapon";
const string kstrGoalType = "goaltype";
void Module::SetParam(const Value &jsn) {
	auto mtype = jsn["type"].asString();
	auto w = jsn["w"].asInt();
	auto h = jsn["h"].asInt();
	auto collw = jsn["collw"].asInt();
	auto collh = jsn["collh"].asInt();

	auto bodyFunc = jsn.isMember("bodyfunc") ? jsn["bodyfunc"].asString() : "SampleFunc";
	auto generate = jsn.isMember("generate_type") ? jsn["generate_type"].asString() : "common";
	auto center = jsn.isMember("center") ? jsn["center"].asBool() : true;
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

	is_center_ = center;
	SetSize(w, h);
	type_ = (mod_type)kModType[mtype];
	gtype_ = (generate_type)kGenerate[generate];
	SetCreateBodyFunc(kCreateBody[bodyFunc]);

	for (auto &ani : jsn["animat"]) {
		auto stat = ani["state"].asString();
		auto pic = ani["pic"].asString();
		auto dt = ani["dt"].asInt();

		AddAssetAnimation(pic, dt, kModDirect[stat]);
	}
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

	if (mod->type_ == role) {
		mod->AddFrameEvent([](void *self) {
			Actor *actor = (Actor *)self;
			auto x = 0, y = 0;
			auto vel = 10;
			if (ns_sdl_winx::EventHandle::Instance()->GetKeyState(SDL_SCANCODE_D)) {
				x = vel;
			}
			if (ns_sdl_winx::EventHandle::Instance()->GetKeyState(SDL_SCANCODE_A)) {
				x = -vel;
			}
			if (ns_sdl_winx::EventHandle::Instance()->GetKeyState(SDL_SCANCODE_W)) {
				y = vel;
			}
			if (ns_sdl_winx::EventHandle::Instance()->GetKeyState(SDL_SCANCODE_S)) {
				y = -vel;
			}
			actor->SetVel(d_vel(x, y));
			
			if (ns_sdl_winx::EventHandle::Instance()->GetMouseState(1)) {
				auto n = 20;
				static int count = 0;
				if (count % n == n-1) {
					auto master = actor;
					int x, y;
					dynamic_cast<ModuleInstance *>(master)->GetSubGeneratePos(x, y);
					ModuleFactory::Instance()->SafeAddToLayer<ModuleInstance>("FollowBullet", master->parent_, master,
												  x, y, "");
					count = 0;
				}
				count++;
			}
		});
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
