#include "combination.h"
#include "module.h"
#include "entity.h"
#include "map.h"

using namespace std;

using namespace ns_engine;
using namespace ns_module;
using namespace ns_entity;
using namespace Json;

namespace ns_combination {

void Combination::Update(const unsigned &dt) {
	UpdateState();
}

const string strGenerateSeq = "seq";
void Combination::SetParam(const Json::Value &jsn) {
	
	if (jsn.isMember(kstrGoalType) && jsn[kstrGoalType].isArray()) {
		for (auto &it : jsn[kstrGoalType]) {
			goaltype_.set((mod_type)kModType[it.asString()]);
		}
	}

	if (jsn.isMember(strGenerateSeq) && jsn[strGenerateSeq].isArray()) {
		for (auto &it : jsn[strGenerateSeq]) {
			g_que_.push(it.asString());
		}
	}

	ai_name_ = JSON_VAL(jsn, kstrAi, String, "");
}

void Combination::UpdateState() {
	frame();
	if (frame(40)) {
		wait_a_frame = false;
	}
	if (!g_que_.empty()) {
		if (frame(30)) {
			auto name = g_que_.front();
			g_que_.pop();
			ModuleFactory::Instance()->SafeAddToLayerByCombination<ModuleInstance>(name, parent_, this, x_, y_);
			wait_a_frame = true;
		}
	} else if (members_.empty()&&g_que_.empty()&&!wait_a_frame) { //创建都没完成不可能空，所以这里用else if判断combination是否死亡
		Death();
	}
	RemoveMember();
}

Generator::Generator() {
}

bool Generator::Drive(Actor *actor) {
	if (ai_name_.empty()) {
		return true;
	}
	auto it = members_.find(actor);
	if (it == members_.end()) {
		shared_ptr<Ai> ai(kAiMap.find(ai_name_) != kAiMap.end() ? kAiMap[ai_name_]() : kExAiMap[ai_name_]->Copy());
		ai->SetMaster(actor);
		//print(dynamic_cast<ModuleInstance*>(actor)->give_skill_);
		AddMember(actor, ai);
		ai->Drive(actor);
		return ai->IsEnd();
	} else {
		auto ai = ((Ai *)(it->second.get()));
		ai->Drive(actor);
		return ai->IsEnd();
	}
}

};
