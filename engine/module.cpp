#include "module.h"

using namespace ns_engine;
using namespace Json;
namespace ns_module {

MTYPE_MAP_BEG(kModType)
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

MTYPE_MAP_BEG(kModDirect)
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

Actor *CreateMod(Game *wx, const string &asstpath, 
	const string &strJsn, Layer *layer, bool isTest /*= false*/) {
	if (!layer) {
		return nullptr;
	}
	Value jsn;
	Reader rd;
	auto isJsn = rd.parse(strJsn, jsn);
	if (!isJsn) {
		return nullptr;
	}
	lock_guard<mutex> lk(wx->mtx_);
	auto mtype = jsn["type"].asString();
	auto x = jsn["x"].asInt();
	auto y = jsn["y"].asInt();
	auto w = jsn["w"].asInt();
	auto h = jsn["h"].asInt();
	auto collw = jsn["collw"].asInt();
	auto collh = jsn["collh"].asInt();

	auto mod = new Actor(SampleFunc);
	layer->AddSub(mod);

	if (isTest) {
		x = y = 0;

		x += layer->w_ / 2;
		y += layer->h_ / 2;
	}

	bool center = true;
	if (center) {
		x -= w / 2, y -= h / 2;
	}
	mod->SetPostion(x, y);
	mod->SetSize(w, h);
	layer->RelateSub(mod);

	auto generate = jsn["generate_type"].asString();
	for (auto &ani : jsn["animat"]) {
		auto stat = ani["state"].asString();
		auto pic = ani["pic"].asString();
		auto dt = ani["dt"].asInt();

		auto path = asstpath + "\\" + pic;

		mod->AddAssetAnimation(path, dt, kModDirect[stat]);
	}

	if (kModType[mtype] == mod_type::role) {
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
			if (ns_sdl_winx::EventHandle::Instance()->GetKeyState(SDL_SCANCODE_R)) {
				actor->angle_++;
			}
			actor->SetVel(d_vel(x, y));
		});
	}
	return mod;
}
};
