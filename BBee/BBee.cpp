#include <engine.h>

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
	auto wx = Game::Instance();
	wx->Create(width, height);

	Scene scene;
	Layer layer;

	wx->AddSub(&scene);
	scene.AddSub(&layer);
	scene.SetPostion(0, 0, false);
	scene.SetSize(width, height);
	layer.SetPostion(0, 0, false);
	layer.SetSize(width, height);

	MainCamera::Instance()->SetPostion(0, 0, false);
	MainWorld::Instance()->SetDbgDraw(MainCamera::Instance());
	MainWorld::Instance()->SetGravity(b2Vec2(0, 0));

	Actor actor(SampleFunc);
	AddActor(layer, actor, 5, 5, 50, 40);

	Actor second(SampleFunc);
	AddActor(layer, second, 55, 55, 50, 40);

	Actor limit_bottom;
	AddActorWithoutTexture(layer, limit_bottom, 0, 0, width, 1);

	Actor limit_top;
	AddActorWithoutTexture(layer, limit_top, 0, height, width, 1);

	Actor limit_left;
	AddActorWithoutTexture(layer, limit_left, 0, 0, 1, height);

	Actor limit_right;
	AddActorWithoutTexture(layer, limit_right, width-1, 0, 1, height);

	bx2Collision<Actor> collision;
	MainWorld::Instance()->World()->SetContactListener(&collision);
	
	layer.CameraFollow(0, &limit_bottom, false);

	actor.AddFrameEvent([](void *self) {
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

	/*actor.AddFrameEvent([](void *self) {
		Actor *actor = (Actor *)self;
		actor->y_++;		
	});*/

	wx->Run();
}