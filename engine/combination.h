#ifndef COMBINATION_H
#define COMBINATION_H
#include "engine.h"
#include "bx2-engine.h"
#include "ai-base.h"
#include "json/json.h"
#include "ai.h"

namespace ns_combination {
using namespace std;
using namespace ns_engine;
using namespace ns_box2d;
using namespace ns_module;

class Combination : public Actor, public MultAi {
public:
	void Update(const unsigned &dt);
	virtual void UpdateState();

	//为了适应ModuleFactory模板的LoadModule和Copy函数
	virtual void Init() {}
	virtual void SetParam(const Json::Value &jsn);

	const string &AiName() { return ai_name_; }
protected:
	string ai_name_;
	queue<string> g_que_;
	bool wait_a_frame = false;

private:
};

class Generator : public Combination {
public:
	Generator();
	bool Drive(Actor *actor);

	void push(const string &x) { g_que_.push(x); }

protected:
private:
	Ai *ai = nullptr;

};

using CombinationInstance = Generator;
};
#endif
