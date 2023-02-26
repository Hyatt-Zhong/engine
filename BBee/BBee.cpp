#include <engine.h>
#include <thread>
#include <entity.h>
#include <ai.h>
#include <menu.h>
#include <physic-module.h>
#include <map.h>

using namespace ns_engine;
using namespace ns_box2d;
using namespace placeholders;
using namespace ns_menu;
using namespace ns_physic_module;
using namespace ns_map;
using namespace ns_entity;

void AddButton(Layer &layer, Actor &actor, int x, int y, int w, int h, bool center = false) {
	layer.AddSub(&actor);
	actor.SetSize(w, h);
	actor.SetPostion(x, y);
}


const int width = 1000;
const int height = 600;

#undef main
void main() {
	auto wx = Game::Instance();
	wx->SetPath("D:\\P\\game-bee");
	wx->Create(width, height);
	wx->SetDieFlag(false);
	wx->OpenAudio();

	ModuleFactory::Instance()->LoadModules(wx->GetModulePath());
	MapManager::Instance()->LoadMaps(wx->GetMapPath());

	auto mapset = [](Map *map) {
		map->SetRoleCreateEvent([](void *role) {
			auto actor = (Actor *)role;
			actor->AddKeyEvent(SDLK_t, [=](const SDL_Keycode &key, const Uint32 &up_or_down, void *data) {
				if (up_or_down == SDL_KEYUP) {
					auto master = actor;
					int x, y;
					dynamic_cast<ModuleInstance *>(master)->GetSubGeneratePos(x, y);
					ModuleFactory::Instance()->SafeAddToLayer<ModuleInstance>("FollowBullet", master->parent_, master,
												  x, y, "");
				}
			});
			actor->AddFrameEvent([](void *self) {
				ns_menu::Blood::BloodData data;
				auto rolex = (ModuleInstance *)self;
				data.val = rolex->life_;
				data.max = rolex->maxlife_;
				Game::Instance()->OnNotice("info", "bd_role", &data);
			});

			actor->AddFrameEvent([](void *self) {
				Actor *actor = (Actor *)self;
				auto x = 0, y = 0;
				if (ns_sdl_winx::EventHandle::Instance()->GetKeyState(SDL_SCANCODE_D)) {
					x = 1;
				}
				if (ns_sdl_winx::EventHandle::Instance()->GetKeyState(SDL_SCANCODE_A)) {
					x = -1;
				}
				if (ns_sdl_winx::EventHandle::Instance()->GetKeyState(SDL_SCANCODE_W)) {
					y = 1;
				}
				if (ns_sdl_winx::EventHandle::Instance()->GetKeyState(SDL_SCANCODE_S)) {
					y = -1;
				}
				d_vel xv(x, y);
				xv.Normalize();
				xv *= dynamic_cast<ModuleInstance *>(actor)->velocity_;
				actor->SetVel(xv);

				if (ns_sdl_winx::EventHandle::Instance()->GetMouseState(1)) {
					auto n = 20;
					static int count = 0;
					if (count % n == n - 1) {
						auto master = actor;
						int x, y;
						dynamic_cast<ModuleInstance *>(master)->GetSubGeneratePos(x, y);
						ModuleFactory::Instance()->SafeAddToLayer<ModuleInstance>("FollowBullet", master->parent_,
													  master, x, y, "");
						count = 0;
					}
					count++;
				}
			});
		});
	};

	Scene sc_start("start");
	Menu mn_start("start");

	wx->AddSub(&sc_start);
	sc_start.AddSub(&mn_start);
	sc_start.SetPostion(0, 0, false);
	sc_start.SetSize(width, height);
	mn_start.SetPostion(0, 0, false);
	mn_start.SetSize(width, height);

	/*EventButton ac_start([mapset]() {
		Game::Instance()->SwitchScene("play");
		auto lv1 = MapManager::Instance()->CopyMap("lv1");
		mapset(lv1);
		auto ly_play = Game::Instance()->GetLayer("play");
		ly_play->SetMap(lv1);
	});*/
	EventButton ac_end([wx]() { wx->ShutDown(); });
	//AddButton(mn_start, ac_start, 200, 200, 100, 50);
	AddButton(mn_start, ac_end, 200, 100, 100, 50);
	//ac_start.AddAssetAnimation("start.png", 0, 0);
	ac_end.AddAssetAnimation("end.png", 0, 0);

	EventButton ac_lv1([mapset]() {
		Game::Instance()->SwitchScene("play");
		auto lv1 = MapManager::Instance()->CopyMap("lv1");
		mapset(lv1);
		auto ly_play = Game::Instance()->GetLayer("play");
		ly_play->SetMap(lv1);
	});
	EventButton ac_lv2([mapset]() {
		Game::Instance()->SwitchScene("play");
		auto lv2 = MapManager::Instance()->CopyMap("lv2");
		mapset(lv2);
		auto ly_play = Game::Instance()->GetLayer("play");
		ly_play->SetMap(lv2);
	});
	EventButton ac_lv3([mapset]() {
		Game::Instance()->SwitchScene("play");
		auto lv3 = MapManager::Instance()->CopyMap("lv3");
		mapset(lv3);
		auto ly_play = Game::Instance()->GetLayer("play");
		ly_play->SetMap(lv3);
	});

	AddButton(mn_start, ac_lv1, 200, 400, 200, 200);
	ac_lv1.AddAssetAnimation("lv1.png", 0, 0);
	AddButton(mn_start, ac_lv2, 500, 400, 200, 200);
	ac_lv2.AddAssetAnimation("lv2.png", 0, 0);
	AddButton(mn_start, ac_lv3, 800, 400, 200, 200);
	ac_lv3.AddAssetAnimation("lv3.png", 0, 0);

	Scene sc_play("play");
	Layer ly_play("play");
	wx->AddSub(&sc_play);
	sc_play.AddSub(&ly_play);
	sc_play.SetPostion(0, 0, false);
	sc_play.SetSize(width, height);
	ly_play.SetPostion(0, 0, false);
	ly_play.SetSize(width, height);

	sc_start.SetAlive(true);
	ly_play.SetWorld(MainWorld::Instance());
	
	//ly_play.CameraFollow(0, "leadrole");

	Menu mn_pause("pause");
	sc_play.AddSub(&mn_pause);
	mn_pause.SetPostion(0, 0, false);
	mn_pause.SetSize(width, height);

	EventButton ac_continue([]() { Game::Instance()->ShowLayer("pause", false); });
	EventButton ac_gomain([]() {
		auto ly_play = Game::Instance()->GetLayer("play");
		auto map = ly_play->GetMap();
		ly_play->ForceClear();
		map->Clear();
		SAFE_DELETE(map);
		Game::Instance()->SwitchScene("start");
	});
	AddButton(mn_pause, ac_continue, 200, 200, 150, 50);
	AddButton(mn_pause, ac_gomain, 200, 100, 150, 50);
	ac_continue.AddAssetAnimation("continue.png", 0, 0);
	ac_gomain.AddAssetAnimation("main-menu.png", 0, 0);
	mn_pause.InitAlive(false);

	Layer ly_info("info");
	sc_play.AddSub(&ly_info);
	ly_info.SetPostion(0, 0, false);
	ly_info.SetSize(width, height);

	Blood bd_role("bd_role");
	{
		int x = 0, h = 10, y = height - h, w = 1;
		AddButton(ly_info, bd_role, x, y, w, h);
		bd_role.AddAssetAnimation("blood.png", 0, 0);
	}

	Blood bd_boss("bd_boss");
	{
		bd_boss.Center();
		int x = 0, h = 10, y = height * 0.9 - h, w = 0;
		AddButton(ly_info, bd_boss, x, y, w, h);
		bd_boss.AddAssetAnimation("blood.png", 0, 0);
	}	

	SkillMenu smn("mn_skill");
	sc_play.AddSub(&smn);

	MainCamera::Instance()->SetPostion(0, 0, false);
	//MainWorld::Instance()->SetDbgDraw(MainCamera::Instance());
	MainWorld::Instance()->SetGravity(b2Vec2(0, 0));

	bx2Collision<Actor> collision;
	MainWorld::Instance()->World()->SetContactListener(&collision);

	Game::ListenKey();
	Game::ListenMouse();	

	ly_play.AddKeyEvent(SDLK_ESCAPE, [](const SDL_Keycode &key, const Uint32 &up_or_down, void *data) {
		if (up_or_down == SDL_KEYUP) {
			auto sc = ((Layer *)(data))->parent_;
			if (sc->IsAlive()) {
				sc->ShowLayer("pause", true);
			}
		}
	});

	ly_play.AddKeyEvent(SDLK_SPACE, [](const SDL_Keycode &key, const Uint32 &up_or_down, void *data) {
		if (up_or_down == SDL_KEYUP) {
		}
	});

	Layer ly_settlement("settlement");
	sc_play.AddSub(&ly_settlement);
	ly_settlement.SetPostion(0, 0, false);
	ly_settlement.SetSize(width, height);
	ly_settlement.InitAlive(false);

	ly_play.AddFrameEvent([](void *self) {
		auto ly = (Layer *)self;
		if (ly->IsEnd()) {
			auto sc = ly->parent_;
			if (sc->IsAlive()) {
				sc->ShowLayer("settlement", true, false);
			}
		}
	});

	EventButton ac_settlement([]() {
		auto ly_play = Game::Instance()->GetLayer("play");
		auto map = ly_play->GetMap();
		ly_play->ForceClear();
		map->Clear();
		SAFE_DELETE(map);
		Game::Instance()->SwitchScene("start");
	});
	AddButton(ly_settlement, ac_settlement, 200, 100, 150, 50);
	ac_settlement.AddAssetAnimation("main-menu.png", 0, 0);

	Label lb;
	AddButton(ly_settlement, lb, width/2, height/2, 250, 150);
	lb.AddAssetAnimation("win.png", 0, 0);

	/*mn_start.AddFrameEvent([=](void *self) {
		if (ns_sdl_winx::EventHandle::Instance()->GetMouseState(1)) {
			wx->DrawLine(0, 0, 300, 300);
		}
	});*/

	/*Label elc1;
	AddButton(mn_start, lb, width / 2, height / 2, 10, 200);
	lb.AddAssetAnimation("elc1.png", 15, 0);
	lb.AddAssetAnimation("elc2.png", 15, 0);
	lb.AddAssetAnimation("elc3.png", 15, 0);*/

	wx->Run();

	wx->Destroy();
	ModuleFactory::Instance()->UnLoadModules();
	MapManager::Instance()->UnLoadMaps();
	wx->ReleaseAsset();
}
