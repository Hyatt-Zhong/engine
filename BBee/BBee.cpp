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

void AddActorWithoutTexture(Layer &layer, Actor &actor, int x, int y, int w, int h, bool center = false) {
	layer.AddSub(&actor);
	if (center) {
		x -= w / 2, y -= h / 2;
	}
	actor.SetPostion(x, y);
	actor.SetSize(w, h);
	if (layer.world_) {
		layer.world_->RelateWorld(&actor);
	}
}

void AddActor(Layer& layer, Actor& actor, int x, int y, int w, int h,bool center=false) {
	
	AddActorWithoutTexture(layer, actor, x, y, w, h, center);
	actor.AddAssetAnimation("player.png", 300, 0);
	//actor.AddAssetAnimation("actor1.png", 300, 0);
	//actor.AddAssetAnimation("actor1x.png", 300, 0);
	//actor.AddAssetAnimation("actorx.png", 300, 0);
	actor.AddAssetAnimation("player.png", 300, 0);
}

void AddButton(Layer &layer, Actor &actor, int x, int y, int w, int h, bool center = false) {
	layer.AddSub(&actor);
	if (center) {
		x -= w / 2, y -= h / 2;
	}
	actor.SetPostion(x, y);
	actor.SetSize(w, h);
}


const int width = 1000;
const int height = 600;

#undef main
void main() {
	auto wx = Game::Instance();
	wx->SetPath("D:\\P\\game-bee");
	wx->Create(width, height);
	wx->SetDieFlag(false);

	Scene sc_start("start");
	Menu mn_start("start");

	wx->AddSub(&sc_start);
	sc_start.AddSub(&mn_start);
	sc_start.SetPostion(0, 0, false);
	sc_start.SetSize(width, height);
	mn_start.SetPostion(0, 0, false);
	mn_start.SetSize(width, height);

	Link ac_start("play", Link::LinkType::kScene);
	EventButton ac_end;
	AddButton(mn_start, ac_start, 200, 200, 100, 50);
	AddButton(mn_start, ac_end, 200, 100, 100, 50);
	ac_start.AddAssetAnimation("start.png", 0, 0);
	ac_end.AddAssetAnimation("end.png", 0, 0);

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

		Actor limit_bottom;
	AddActorWithoutTexture(ly_play, limit_bottom, 0, 0, width, 1);
	
		Actor limit_top;
	AddActorWithoutTexture(ly_play, limit_top, 0, height, width, 1);
	
		Actor limit_left;
	AddActorWithoutTexture(ly_play, limit_left, 0, 0, 1, height);
	
		Actor limit_right;
	AddActorWithoutTexture(ly_play, limit_right, width - 1, 0, 1, height);
	//sc_play.SetAlive(true);
	ModuleFactory::Instance()->LoadModules(wx->GetModulePath());
	//TestMap tmap;
	//tmap.AddTestMod("assistant", "assistant"); //必须加后面的名字
	//tmap.AddTestMod("assistant", "assistant1", width / 2, height / 2); //必须加后面的名字
	//tmap.AddTestMod("assistant", "assistant2", width / 2, height);     //必须加后面的名字
	//tmap.AddTestMod("assistant", "assistant4", width / 2, height/3);     //必须加后面的名字
	//ly_play.SetMap(&tmap);
	MapManager::Instance()->LoadMaps(wx->GetMapPath());
	auto lv1 = MapManager::Instance()->GetMap("lv1");
	lv1->SetRoleCreateEvent([](void *role) {
		auto actor = (Actor *)role;
		actor->AddKeyEvent(SDLK_t, [=](const SDL_Keycode &key, const Uint32 &up_or_down, void *data) {
			if (up_or_down == SDL_KEYUP) {
				auto master = actor;
				int x, y;
				dynamic_cast<ModuleInstance *>(master)->GetSubGeneratePos(x, y);
				ModuleFactory::Instance()->SafeAddToLayer<ModuleInstance>("FollowBullet", master->parent_, master, x, y,
											  "");
			}
		});
		actor->AddFrameEvent([](void *self) {
			auto data = new ns_menu::Blood::BloodData;
			auto rolex = (ModuleInstance *)self;
			data->val = rolex->life_;
			data->max = rolex->maxlife_;
			Game::Instance()->OnNotice("info", "bd_role", data);
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
					ModuleFactory::Instance()->SafeAddToLayer<ModuleInstance>("FollowBullet", master->parent_, master,
												  x, y, "");
					count = 0;
				}
				count++;
			}
		});
	});
	ly_play.SetMap(lv1);
	//ly_play.CameraFollow(0, "leadrole");

	Menu mn_pause("pause");
	sc_play.AddSub(&mn_pause);
	mn_pause.SetPostion(0, 0, false);
	mn_pause.SetSize(width, height);

	Link ac_continue("pause", false);
	Link ac_gomain("start", Link::LinkType::kScene);
	AddButton(mn_pause, ac_continue, 200, 200, 100, 50);
	AddButton(mn_pause, ac_gomain, 200, 100, 100, 50);
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
		int x = 0, h = 10, y = height * 0.9 - h, w = 10;
		AddButton(ly_info, bd_boss, x, y, w, h);
		bd_boss.AddAssetAnimation("blood.png", 0, 0);
	}	

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

	wx->Run();
}
	//void main() {
//	struct data_pack {
//		Game *xwx = nullptr;
//		Layer xlayer;
//		Scene xscene;
//		int x, y, w, h;
//		HWND parent;
//
//		Actor *xmod = nullptr;
//
//		condition_variable cv;
//		mutex mtx;
//		void Init(int xx, int xy, int xw, int xh, HWND hwnd) {
//			parent = hwnd;
//			x = xx, y = xy, w = xw, h = xh;
//		}
//	};
//	auto xdp = new data_pack;
//
//	auto xfunc = [](data_pack *xdp) {
//		xdp->xwx = Game::Instance();
//		auto wx = xdp->xwx;
//		wx->Create(width, height);
//
//		auto scene = &xdp->xscene;
//		auto layer = &xdp->xlayer;
//
//		wx->AddSub(scene);
//		scene->AddSub(layer);
//		scene->SetPostion(0, 0, false);
//		scene->SetSize(width, height);
//		layer->SetPostion(0, 0, false);
//		layer->SetSize(width, height);
//
//		Actor enemy(NoCollFunc);
//		AddActor(*layer, enemy, 100, 100, 50, 40);
//		Follow fl;
//		MoveLeft ml(4.f);
//		MoveRight mr(4.f);
//		MoveUp mu;
//		MoveDown md;
//		enemy.PushAi(&ml);
//		enemy.PushAi(&mr);
//		enemy.PushAi(&mr);
//		enemy.PushAi(&ml);
//		enemy.PushAi(&mu);
//		enemy.PushAi(&md);
//		enemy.PushAi(&md);
//		enemy.PushAi(&mu);
//		//enemy.PushAi(&fl);
//		enemy.goaltype_.set(ns_module::mod_type::role);
//		enemy.type_ = ns_module::mod_type::enemy;
//
//		Actor enemy1(NoCollFunc);
//		AddActor(*layer, enemy1, 400, 5, 80, 64);
//		/*Follow flx;
//		Clockwise cw(200);
//		enemy1.PushAi(&cw);
//		enemy1.PushAi(&flx);*/
//		Circle crl(15);
//		crl.SetClockwise(false);
//		enemy1.PushAi(&crl);
//		enemy1.goaltype_.set(ns_module::mod_type::role);
//		enemy1.type_ = ns_module::mod_type::enemy;
//
//		MainCamera::Instance()->SetPostion(0, 0, false);
//		//MainWorld::Instance()->SetDbgDraw(MainCamera::Instance());
//		MainWorld::Instance()->SetGravity(b2Vec2(0, 0));
//
//		wx->Run();
//
//		delete xdp;
//	};
//
//	auto th = new thread(xfunc, xdp);
//	th->detach();
//
//	Sleep(500);
//
//	auto asstpath = "D:\\P\\game-workplace\\first-arpg\\asset";
//	auto mod = read_file("D:\\P\\game-workplace\\first-arpg\\module\\role\\jet.json");
//	auto role = ns_module::CreateMod(xdp->xwx, asstpath, mod, &xdp->xlayer, true);
//	role->type_ = ns_module::mod_type::role;
//
//	ns_menu::Blood bld("xxx", [](void *a, void *x) {});
//
//	system("pause");
//}

//void main() {
//	auto wx = Game::Instance();
//	wx->SetPath("D:\\P\\project-engine\\out");
//	wx->Create(width, height);
//
//	Scene scene;
//	Layer layer;
//
//	wx->AddSub(&scene);
//	scene.AddSub(&layer);
//	scene.SetPostion(0, 0, false);
//	scene.SetSize(width, height);
//	layer.SetPostion(0, 0, false);
//	layer.SetSize(width, height);
//
//	MainCamera::Instance()->SetPostion(0, 0, false);
//	MainWorld::Instance()->SetDbgDraw(MainCamera::Instance());
//	MainWorld::Instance()->SetGravity(b2Vec2(0, 0));
//
//	Actor actor(NoCollFunc);
//	AddActor(layer, actor, 5, 5, 50, 40);
//
//	Actor second(NoCollFunc);
//	AddActor(layer, second, 55, 55, 50, 40);
//
//	Actor limit_bottom;
//	AddActorWithoutTexture(layer, limit_bottom, 0, 0, width, 1);
//
//	Actor limit_top;
//	AddActorWithoutTexture(layer, limit_top, 0, height, width, 1);
//
//	Actor limit_left;
//	AddActorWithoutTexture(layer, limit_left, 0, 0, 1, height);
//
//	Actor limit_right;
//	AddActorWithoutTexture(layer, limit_right, width-1, 0, 1, height);
//
//	bx2Collision<Actor> collision;
//	MainWorld::Instance()->World()->SetContactListener(&collision);
//	
//	layer.CameraFollow(0, &limit_bottom, false);
//
//	actor.AddFrameEvent([](void *self) {
//		Actor *actor = (Actor *)self;
//		auto x = 0, y = 0;
//		auto vel = 10;
//		if (ns_sdl_winx::EventHandle::Instance()->GetKeyState(SDL_SCANCODE_D)) {
//			x = vel;
//		}
//		if (ns_sdl_winx::EventHandle::Instance()->GetKeyState(SDL_SCANCODE_A)) {
//			x = -vel;
//		}
//		if (ns_sdl_winx::EventHandle::Instance()->GetKeyState(SDL_SCANCODE_W)) {
//			y = vel;
//		}
//		if (ns_sdl_winx::EventHandle::Instance()->GetKeyState(SDL_SCANCODE_S)) {
//			y = -vel;
//		}
//		actor->SetVel(d_vel(x, y));
//	});
//
//	/*actor.AddFrameEvent([](void *self) {
//		Actor *actor = (Actor *)self;
//		actor->y_++;		
//	});*/
//
//	wx->Run();
//}