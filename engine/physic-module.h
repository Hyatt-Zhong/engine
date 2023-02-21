#ifndef PHYSIC_MODULE_H
#define PHYSIC_MODULE_H
#include "engine.h"
#include "bx2-engine.h"
#include "ai-base.h"

namespace ns_physic_module {
using namespace std;
using namespace ns_engine;
using namespace ns_box2d;
using namespace ns_module;

b2Body *NoCollFunc(Actor *actor);
b2Body *CollFunc(Actor *actor);
b2Body *CollNoRestiFunc(Actor *actor);
};
#endif
