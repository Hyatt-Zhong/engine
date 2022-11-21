#include "../engine/sdl_windx.h"
#include "../engine/engine.h"

using namespace ns_engine;
using namespace ns_box2d;
using namespace placeholders;

class bullet:public xActor
{
public:
	void OnCollision(Actor *actor) {
		print("collision with", actor);
		if (actor) {
			((xActor*)actor)->is_destroy_ = true;
		}
	}

protected:
private:
};


void AddActor(Layer& layer, Actor& actor, int x, int y, int w, int h,bool center=false) {
	layer.AddSub(&actor);
	if (center) {
		x -= w / 2, y -= h / 2;
	}
	actor.SetPostion(x, y);
	actor.SetSize(w, h);
	if (layer.world_) {
		layer.world_->RelateWorld(&actor);
	}

	actor.AddAssetAnimation("D:\\P\\project-engine\\out\\asset\\actor1.png", 300, 0);
	actor.AddAssetAnimation("D:\\P\\project-engine\\out\\asset\\actor1x.png", 300, 0);
	actor.AddAssetAnimation("D:\\P\\project-engine\\out\\asset\\actorx.png", 300, 0);
	actor.AddAssetAnimation("D:\\P\\project-engine\\out\\asset\\actorr.png", 300, 0);
}

#undef main
void main()
{
	auto wx = Game::Instance();
	wx->Create(500, 400);

	Scene scene;
	Layer layer;
	Actor actor(SampleFunc);

	wx->AddSub(&scene);
	scene.AddSub(&layer);
	scene.SetPostion(0, 0, false);
	scene.SetSize(500, 400);
	layer.SetPostion(0, 0, false);
	layer.SetSize(300, 200);

	ns_engine::MainCamera::Instance()->SetPostion(0, 0, false);

	Camera camera(wx);
	camera.SetViewport(500, 400);
	layer.SetCamera(&camera);
	layer.camera_->SetPostion(0, 0);

	MainWorld::Instance()->SetDbgDraw(layer.camera_);

	MainWorld::Instance()->SetGravity(b2Vec2(0, -100));
	MainWorld::Instance()->CreateStaticBody(0, -90, 300, 10);


	//scene.AddAssetAnimation("D:\\P\\project-engine\\out\\asset\\scene.png", 0, 0);
	//layer.AddAssetAnimation("D:\\P\\project-engine\\out\\asset\\layer.png", 0, 0);

	AddActor(layer, actor, 0, 0, 50, 40);
	Actor second(SampleFunc);
	AddActor(layer, second, 80, 80, 50, 40);

	Layer menu;
	Actor button;
	scene.AddSub(&menu);
	menu.SetPostion(0, 0, false);
	menu.SetSize(300, 200);

	bx2World menu_world;
	wx->AddWorld(&menu_world);
	menu_world.SetDbgDraw(ns_engine::MainCamera::Instance());
	menu.SetWorld(&menu_world);

	//menu.AddAssetAnimation("D:\\P\\project-engine\\out\\asset\\layer.png", 0, 0);
	AddActor(menu, button, 0, 0, 50, 40);

	bx2Collision<Actor> collision;

	MainWorld::Instance()->World()->SetContactListener(&collision);

	wx->LoadAsset();

	//ns_sdl_winx::EventHandle::Instance()->SetMouseLUpHandle(bind(
	//	[](auto &&x, auto &&y, auto &&button, Actor *actor,
	//	   Layer *layer) {
	//		if (button == 1) {//left button
	//			actor->SetPostionByMouse(x, y);
	//			layer->OnClick(x, y);
	//		}
	//	},
	//	_1, _2, _3, &actor, &menu));

	ns_sdl_winx::EventHandle::Instance()->SetMouseLUpHandle(bind(
		[](auto &&xx, auto &&yy, auto &&button, Layer* layer) {
			if (button == 1) { //left button
				auto actor = new bullet;
				actor->SetCreateBodyFunc(SampleFunc);
				auto x = xx, y = yy;
				layer->camera_->Trans(x, y);
				AddActor(*layer, *actor, x, y, 50, 40, true);
				//MainWorld::Instance()->RelateWorld(actor);
			}
		},
		_1, _2, _3, &layer));

	ns_sdl_winx::EventHandle::Instance()->AddKeyUpHandle(SDLK_x,
		bind([](Actor* actor) {actor->SetPostion(0, 0); }
	, &actor));

	ns_sdl_winx::EventHandle::Instance()->AddKeyDownHandle(SDLK_c,
		bind([](Actor* actor) {actor->SetPostion(0, 0); }
	, &actor));

	ns_sdl_winx::EventHandle::Instance()->AddMouseHandle(
		SDL_MOUSEBUTTONDOWN,
		bind(
			[](const int &x, const int &y, const Uint8 &button,
			   Layer *layer) {
				print("mouse down", int(button));
				if (button == 1) { //left button
					layer->OnClick(x, y);
				}
			},
			_1, _2, _3, &menu));

	actor.AddFrameEvent([](void* self) {
		if (ns_sdl_winx::EventHandle::Instance()->GetKeyState(SDL_SCANCODE_X)) {
			Actor *actor = (Actor *)self;
			print(actor->x_, actor->y_);
		}
		auto index = 3;
		if (ns_sdl_winx::EventHandle::Instance()->GetMouseState(index)) {
			//print(index, "button down");
		}
	});

	int x = 0, y = 0;
	menu.AddFrameEvent(bind(
		[](void *self, int &x, int &y) {
			auto layer = (Layer *)self;
			auto index = 3;
			auto m = x, n = y;
			if (ns_sdl_winx::EventHandle::Instance()->GetMouseState(
				    index, &x, &y)) {
				auto dx = x - m;
				auto dy = y - n;
				print(dx, dy);
				layer->camera_->Move(dx, -dy); //屏幕坐标变换值和引擎坐标变换值是正负是相反的
			}
		},
		_1, x, y));

	wx->Run();
}