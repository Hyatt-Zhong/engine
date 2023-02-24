#ifndef EFFECT_H
#define EFFECT_H
#include "engine.h"
#include "bx2-engine.h"
#include "ai-base.h"
#include "json/json.h"

namespace ns_effect {
using namespace std;
using namespace ns_engine;
using namespace ns_box2d;
using namespace ns_module;

class Effect:public Actor
{
public:
	void ChangeState();
	void Update(const unsigned &dt);

	virtual void Init() {}
	virtual void SetParam(const Json::Value &jsn);

protected:
	bool is_show_ = false;
	string audio_;

private:
};

using EffectInstance = Effect;
};
#endif
