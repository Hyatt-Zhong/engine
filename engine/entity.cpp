#include "entity.h"
#include "macro-def.h"

using namespace ns_engine;
namespace ns_entity {
Entity::Entity() {
}
Entity::~Entity() {
	for (auto &it : wps_) {
		SAFE_DELETE(it);
	}
	wps_.clear();
	for (auto &it : ais_) {
		SAFE_DELETE(it);
	}
	ais_.clear();
	for (auto& it:mais_)
	{
		SAFE_DELETE(it);
	}
	mais_.clear();
}
void Entity::Init() {
	Module::Init();

	for (auto &it : wp_names_) {
		if (!it.empty()) {
			auto wp = kWeaponMap[it]();
			wp->SetMaster(this);
			wps_.push_back(wp);
		}
	}
	for (auto &it : ai_names_) {
		if (!it.empty()) {
			auto ai = kAiMap[it]();
			ai->SetMaster(this);
			PushAi(ai);
			ais_.push_back(ai);
		}
	}

	for (auto &it : mai_names_) {
		if (!it.empty()) {
			auto ai = kMAiMap[it]();
			ai->SetMaster(this);
			ai_control_[it] = ai;
			mais_.push_back(ai);
		}
	}
}

const string kstrMaxLife = "maxlife";
const string kstrDamage = "damage";
const string kstrDefense = "defense";
const string kstrVelocity = "velocity";
const string kstrRealDamage = "realdamage";
void Entity::SetParam(const Json::Value &jsn) {
	Module::SetParam(jsn);
	auto xmaxlife = JSON_VAL(jsn, kstrMaxLife, Double, MINUS1);
	auto xdamage = JSON_VAL(jsn, kstrDamage, Double, MINUS1);
	auto xdefense = JSON_VAL(jsn, kstrDefense, Double, MINUS1);
	auto xvelocity = JSON_VAL(jsn, kstrVelocity, Double, MINUS1);
	auto xreal_damage = JSON_VAL(jsn, kstrRealDamage, Double, MINUS1);

	if (xmaxlife != MINUS1) {
		maxlife_ = xmaxlife;
		life_ = maxlife_;
	}
	if (xdamage != MINUS1) {
		damage_ = xdamage;
	}
	if (xdefense != MINUS1) {
		defense_ = xdefense;
	}
	if (xvelocity != MINUS1) {
		velocity_ = xvelocity;
	}
	if (xreal_damage != MINUS1) {
		real_damage_ = xreal_damage;
	}

}
void Entity::Update(const unsigned &dt) {
	if (life_ <= 0) {
		Death();
	}
	if (use_weapon_) {
		for (auto &wp : wps_) {
			wp->Use();
		}
	}
	Actor::Update(dt);
}

void Entity::OnCollision(Actor *actor) {
	auto other = dynamic_cast<ModuleInstance *>(actor);
	if (other->goaltype_.test(type_)) {
		auto real_damage = other->damage_ - defense_;
		life_ -= real_damage > 0 ? real_damage : 0;
		life_ -= other->real_damage_;

		//我们会设置子弹的敌人为某物，但是不会设置某物的敌人为子弹，所以要做以下操作
		if (other->type_ == bullet) {
			other->life_ -= damage_;
		}
	}
}
void Entity::GetSubGeneratePos(int &x, int &y) {
	auto pos = direct_;
	pos *= (w_ + h_) / 2;
	auto [xx, yy] = GetCenter();
	x = pos.x + xx;
	y = pos.y + yy;
}
};
