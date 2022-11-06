
#include "../engine/engine.h"

#undef main
void main()
{
	Direct direct;
	Scene scene;
	Layer layer;
	Actor actor;

	direct.AddSub(&scene);
	scene.AddSub(&layer);
	scene.AddSub(&layer);
	scene.AddSub(&layer);
	layer.AddSub(&actor);
	layer.AddSub(&actor);
	layer.AddSub(&actor);
	layer.AddSub(&actor);

	direct.Update();
}
