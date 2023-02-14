#include "module.h"
#include "physic-module.h"

using namespace ns_engine;
using namespace Json;
using namespace ns_physic_module;

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


void ModuleFactory::LoadModule(const string &modpath) {
	auto strJsn = read_file(modpath);
	if (strJsn.empty()) {
		return;
	}
	Value jsn;
	Reader rd;
	auto isJsn = rd.parse(strJsn, jsn);
	if (!isJsn) {
		return ;
	}
	auto name = jsn["modname"].asString();
	auto mtype = jsn["type"].asString();
	auto w = jsn["w"].asInt();
	auto h = jsn["h"].asInt();
	auto collw = jsn["collw"].asInt();
	auto collh = jsn["collh"].asInt();

	auto bodyFunc = jsn.isMember("bodyfunc") ? jsn["bodyfunc"].asString() : "SampleFunc";
	auto generate = jsn.isMember("generate_type") ? jsn["generate_type"].asString() : "common";
	auto center = jsn.isMember("center") ? jsn["center"].asBool() : true;

	auto mod = new Module;
	mod->is_center = center;
	mod->SetSize(w, h);
	mod->mtype = (mod_type)kModType[mtype];
	mod->gtype = (generate_type)kGenerate[generate];
	mod->SetCreateBodyFunc(kCreateBody[bodyFunc]);

	for (auto &ani : jsn["animat"]) {
		auto stat = ani["state"].asString();
		auto pic = ani["pic"].asString();
		auto dt = ani["dt"].asInt();

		mod->AddAssetAnimation(pic, dt, kModDirect[stat]);
	}

	mods[name] = mod;
}
void ModuleFactory::LoadModules(const string &modpath) {}
void ModuleFactory::UnLoadModules() {
	for (auto &it : mods) {
		SAFE_DELETE(it.second);
	}
	mods.clear();
}
Actor *ModuleFactory::Copy(const string &name, Layer *layer, const int &x, const int &y) {
	auto mod = new Module(*(Module *)mods[name]);

	layer->AddSub(mod);
	
	mod->SetPostion(x, y, mod->is_center);
	if (layer->world_) {
		layer->world_->RelateWorld(mod);
	}

	if (mod->mtype == role) {
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
		});
	}

	return mod;
}
};
