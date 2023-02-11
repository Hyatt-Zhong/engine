#include <engine.h>
#include <thread>
#include <module.h>
#include <ai.h>

using namespace ns_engine;
using namespace ns_box2d;
using namespace placeholders;

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
	actor.AddAssetAnimation("D:\\P\\project-engine\\out\\asset\\actorw.png", 300, 0);
	//actor.AddAssetAnimation("D:\\P\\project-engine\\out\\asset\\actor1.png", 300, 0);
	//actor.AddAssetAnimation("D:\\P\\project-engine\\out\\asset\\actor1x.png", 300, 0);
	//actor.AddAssetAnimation("D:\\P\\project-engine\\out\\asset\\actorx.png", 300, 0);
	actor.AddAssetAnimation("D:\\P\\project-engine\\out\\asset\\actorr.png", 300, 0);
}


const int width = 500;
const int height = 400;

#undef main
void main() {
	struct data_pack {
		Game *xwx = nullptr;
		Layer xlayer;
		Scene xscene;
		int x, y, w, h;
		HWND parent;

		Actor *xmod = nullptr;

		condition_variable cv;
		mutex mtx;
		void Init(int xx, int xy, int xw, int xh, HWND hwnd) {
			parent = hwnd;
			x = xx, y = xy, w = xw, h = xh;
		}
	};
	auto xdp = new data_pack;

	auto xfunc = [](data_pack *xdp) {
		xdp->xwx = Game::Instance();
		auto wx = xdp->xwx;
		wx->Create(width, height);

		auto scene = &xdp->xscene;
		auto layer = &xdp->xlayer;

		wx->AddSub(scene);
		scene->AddSub(layer);
		scene->SetPostion(0, 0, false);
		scene->SetSize(width, height);
		layer->SetPostion(0, 0, false);
		layer->SetSize(width, height);

		Actor enemy(SampleFunc);
		AddActor(*layer, enemy, 100, 100, 50, 40);
		Follow fl;
		MoveLeft ml(4.f);
		MoveRight mr(4.f);
		MoveUp mu;
		MoveDown md;
		enemy.PushAi(&ml);
		enemy.PushAi(&mr);
		enemy.PushAi(&mr);
		enemy.PushAi(&ml);
		enemy.PushAi(&mu);
		enemy.PushAi(&md);
		enemy.PushAi(&md);
		enemy.PushAi(&mu);
		//enemy.PushAi(&fl);
		enemy.goaltype_.set(ns_module::mod_type::role);
		enemy.type_ = ns_module::mod_type::enemy;

		Actor enemy1(SampleFunc);
		AddActor(*layer, enemy1, 400, 5, 50, 40);
		Follow flx;
		Clockwise cw(200);
		enemy1.PushAi(&cw);
		enemy1.PushAi(&flx);
		enemy1.goaltype_.set(ns_module::mod_type::role);
		enemy1.type_ = ns_module::mod_type::enemy;

		MainCamera::Instance()->SetPostion(0, 0, false);
		MainWorld::Instance()->SetDbgDraw(MainCamera::Instance());
		MainWorld::Instance()->SetGravity(b2Vec2(0, 0));

		wx->Run();

		delete xdp;
	};

	auto th = new thread(xfunc, xdp);
	th->detach();

	Sleep(500);

	auto asstpath = "D:\\P\\game-workplace\\first-arpg\\asset";
	auto mod = read_file("D:\\P\\game-workplace\\first-arpg\\module\\role\\jet.json");
	auto role = ns_module::CreateMod(xdp->xwx, asstpath, mod, &xdp->xlayer, true);
	role->type_ = ns_module::mod_type::role;

	system("pause");
}
//void main() {
//	auto wx = Game::Instance();
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
//	Actor actor(SampleFunc);
//	AddActor(layer, actor, 5, 5, 50, 40);
//
//	Actor second(SampleFunc);
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