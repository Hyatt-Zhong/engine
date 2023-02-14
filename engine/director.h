#ifndef DIRECTOR_H
#define DIRECTOR_H
#include "engine.h"
#include "bx2-engine.h"
#include "ai-base.h"

namespace ns_director {
using namespace std;
using namespace ns_engine;
using namespace ns_box2d;
using namespace ns_module;

struct GameState {
	unsigned state;
	void *data;
};

class Director
{
public:
	Director();
	void LoadAsset();
	void SwitchState();
	void StartState();

protected:
private:
	GameState state;
};
};
#endif
