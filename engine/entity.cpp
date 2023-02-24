#include "entity.h"
#include "macro-def.h"
#include <random>
#include "skill.h"
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
	for (auto &it : mais_) {
		SAFE_DELETE(it);
	}
	mais_.clear();
	for (auto &it : skills_) {
		it.second->UnEquip();
		SAFE_DELETE(it.second);
	}
	skills_.clear();
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
			auto ai = kAiMap.find(it) != kAiMap.end() ? kAiMap[it]() : kExAiMap[it]->Copy();
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
const string kstrDrop = "drop";
const string kstrDropMod = "dropmod";
const string kstrDropChance = "dropchance";
const string kstrDropCount = "dropcount";
const string kstrGiveSkill = "giveskill";
const string kstrAudio = "audio";

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

	if (jsn.isMember(kstrDrop)) {
		auto drop = jsn[kstrDrop];
		drop_ = drop[kstrDropMod].asString();
		drop_chance_ = JSON_VAL(drop, kstrDropChance, Double, 10.);
		drop_count_ = JSON_VAL(drop, kstrDropCount, Int, 1);
	}

	give_skill_ = JSON_VAL(jsn, kstrGiveSkill, String, "");

	if (jsn.isMember(kstrAudio)) {
		auto audio = jsn[kstrAudio];
		auto birth = JSON_VAL(audio, "birth", String, "");
		auto die = JSON_VAL(audio, "die", String, "");
		LoadChunk(birth);
		LoadChunk(die);
		audio_birth_ = birth;
		audio_die_ = die;
	}

	die_effect_ = JSON_VAL(jsn, "dieeffect", String, "");
}
void Entity::Update(const unsigned &dt) {
	if (life_ <= 0) {
		BeKill();
	}
	
	if (use_weapon_) {
		for (auto &wp : wps_) {
			wp->Use();
		}
	}
	for (auto &it : skills_) {
		it.second->Update(dt);
	}
	Actor::Update(dt);
	if (IsBeKill()) {
		PlayChunk(audio_die_);
		Drop();
		DeathEffect();
	}
}

void Entity::OnCollision(Actor *actor) {
	if (!actor) {
		return;
	}
	auto other = dynamic_cast<ModuleInstance *>(actor);
	if ((other->goaltype_ & (type_)) != 0) {
		auto real_damage = other->damage_ - defense_;
		life_ -= real_damage > 0 ? real_damage : 0;
		life_ -= other->real_damage_;
		life_ = life_ >= maxlife_ ? maxlife_ : life_;

		//我们会设置子弹的敌人为某物，但是不会设置某物的敌人为子弹，所以要做以下操作
		if (other->type_ .test( bullet)) {
			other->life_ -= damage_;
		}

		if (!other->give_skill_.empty()) {
			auto skill = other->give_skill_;
			if (!FindSkill(skill)) {
				auto sk = kSkillMap[skill]->Copy();
				PushSkill(skill, sk);
				sk->Equip();
			}
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
void Entity::Drop() {
	if (drop_.empty()) {
		return;
	}
	uniform_int_distribution<unsigned> u(0, 100);
	if (u(re()) <= drop_chance_) {
		auto [xx, yy] = GetCenter();
		ModuleFactory::Instance()->SafeAddToLayer<ModuleInstance>(drop_, parent_, this->goaltype_, xx, yy, drop_count_);
	}
}
void Entity::DeathEffect() {
	if (!die_effect_.empty()) {
		ModuleFactory::Instance()->SafeAddToLayer<EffectInstance>(die_effect_, parent_, this);
	}
}
bool Entity::FindSkill(const string &name) {
	return skills_.find(name) != skills_.end();
}

bool Entity::PushSkill(const string &name, Skill *skill) {
	if (FindSkill(name)) {
		return false;
	}
	skills_[name] = (skill);
	skill->SetMaster(this);
	return true;
}

};
