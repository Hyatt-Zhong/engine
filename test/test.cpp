#include "../engine/sdl_windx.h"
#include "../engine/engine.h"

using namespace ns_engine;
using namespace placeholders;
#undef main
void main()
{
	auto wx = Game::Instance();
	wx->WinInit();
	wx->Create(500, 400);

	wx->Init();

	Scene scene;
	Layer layer;
	Actor actor;

	wx->AddSub(&scene);
	scene.AddSub(&layer);
	layer.AddSub(&actor);
	actor.SetPostion(0, 0);

	scene.AddAssetAnimation("D:\\P\\project-engine\\out\\asset\\scene.png", 0, 0);
	layer.AddAssetAnimation("D:\\P\\project-engine\\out\\asset\\layer.png", 0, 0);
	actor.AddAssetAnimation("D:\\P\\project-engine\\out\\asset\\actor1.png", 300, 0);
	actor.AddAssetAnimation("D:\\P\\project-engine\\out\\asset\\actorx.png", 300, 0);

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

	actor.AddFrameEvent([]() {
		if (ns_sdl_winx::EventHandle::Instance()->GetKeyState(SDL_SCANCODE_X)) {
			print("key x down");
		}
	});

	wx->Run();
}