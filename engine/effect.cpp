#include "effect.h"
#include "module.h"
using namespace ns_engine;
using namespace ns_module;
namespace ns_effect {
void Effect::ChangeState() {
	Actor::ChangeState();
	if (curr_anim_->IsRunEnd()) {
		Death();
	}
}
void Effect::Update(const unsigned &dt) {
	Actor::Update(dt);
	PlayChunk(audio_);
}
void Effect::SetParam(const Json::Value &jsn) {
	if (jsn.isMember(kstrAnimat)) {
		for (auto &ani : jsn[kstrAnimat]) {
			auto stat = ani["state"].asString();
			auto pic = ani["pic"].asString();
			auto dt = ani["dt"].asInt();

			AddAssetAnimation(pic, dt, 0);
		}
	}

	audio_ = JSON_VAL(jsn, "audio", String, "");
	Game::Instance()->LoadChunk(audio_);
}


};
