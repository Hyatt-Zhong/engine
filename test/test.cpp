#include "../engine/sdl_windx.h"
#include "../engine/engine.h"

using namespace ns_engine;
using namespace placeholders;
#undef main
void main()
{
	auto wx = Game::Instance();
	wx->Create(500, 400);

	Scene scene;
	Layer layer;
	Actor actor;

	wx->AddSub(&scene);
	scene.AddSub(&layer);
	layer.AddSub(&actor);
	scene.SetPostion(0, 0, false);
	scene.SetSize(500, 400);
	layer.SetPostion(0, 0, false);
	layer.SetSize(300, 200);
	actor.SetPostion(0, 0);
	actor.SetSize(50, 40);

	wx->MainCamera()->SetPostion(0, 0);

	Camera camera(wx);
	camera.SetViewport(500, 400);
	layer.SetCamera(&camera);
	layer.camera_->SetPostion(-100, -100);

	ns_box2d::MainWorld::Instance()->SetDbgDraw(layer.camera_);

	ns_box2d::MainWorld::Instance()->RelateWorld(&actor);

	//scene.AddAssetAnimation("D:\\P\\project-engine\\out\\asset\\scene.png", 0, 0);
	//layer.AddAssetAnimation("D:\\P\\project-engine\\out\\asset\\layer.png", 0, 0);
	actor.AddAssetAnimation("D:\\P\\project-engine\\out\\asset\\actor1.png", 300, 0);
	actor.AddAssetAnimation("D:\\P\\project-engine\\out\\asset\\actor1x.png", 300, 0);
	actor.AddAssetAnimation("D:\\P\\project-engine\\out\\asset\\actorx.png", 300, 0);
	actor.AddAssetAnimation("D:\\P\\project-engine\\out\\asset\\actorr.png", 300, 0);

	Layer menu;
	Actor button;
	scene.AddSub(&menu);
	menu.AddSub(&button);
	menu.SetPostion(0, 0, false);
	menu.SetSize(300, 200);
	button.SetPostion(0, 0);
	button.SetSize(50, 40);

	ns_box2d::bx2World menu_world;
	wx->AddWorld(&menu_world);
	menu_world.SetDbgDraw(wx->MainCamera());
	menu_world.RelateWorld(&button);

	//menu.AddAssetAnimation("D:\\P\\project-engine\\out\\asset\\layer.png", 0, 0);
	button.AddAssetAnimation("D:\\P\\project-engine\\out\\asset\\actor1.png", 300, 0);
	button.AddAssetAnimation("D:\\P\\project-engine\\out\\asset\\actor1x.png", 300, 0);
	button.AddAssetAnimation("D:\\P\\project-engine\\out\\asset\\actorx.png", 300, 0);
	button.AddAssetAnimation("D:\\P\\project-engine\\out\\asset\\actorr.png", 300, 0);	

	wx->LoadAsset();

	ns_sdl_winx::EventHandle::Instance()->SetMouseLUpHandle(
		bind([](auto&& x, auto&& y, Actor* actor) {actor->SetPostionByMouse(x, y); }
	, _1, _2, &actor));

	ns_sdl_winx::EventHandle::Instance()->AddKeyUpHandle(SDLK_x,
		bind([](Actor* actor) {actor->SetPostion(0, 0); }
	, &actor));

	ns_sdl_winx::EventHandle::Instance()->AddKeyDownHandle(SDLK_c,
		bind([](Actor* actor) {actor->SetPostion(0, 0); }
	, &actor));

	actor.AddFrameEvent([](void* self) {
		if (ns_sdl_winx::EventHandle::Instance()->GetKeyState(SDL_SCANCODE_X)) {
			Actor *actor = (Actor *)self;
			print(actor->x_, actor->y_);
		}
	});

	wx->Run();
}