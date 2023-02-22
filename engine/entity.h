#ifndef ENTITY_H
#define ENTITY_H
#include "module.h"
#include "weapon.h"
namespace ns_skill {
class Skill;
}
namespace ns_entity {
using namespace ns_module;
using namespace ns_weapon;
using namespace ns_skill;

class Entity:public Module
{
public:
	Entity();
	~Entity();
	void Init();
	void SetParam(const Json::Value &jsn);
	void Update(const unsigned &dt);
	void OnCollision(Actor *actor);
	void UpdateCoefficient();
	void GetSubGeneratePos(int &x, int &y);
	void UseWeapon(bool use) { /*use_weapon_ = use;*/ }
	void Drop();
	void DeathEffect();
	bool FindSkill(const string &name);
	bool PushSkill(const string &name, Skill *skill);
	//
	string drop_;
	float drop_chance_ = 10;
	int drop_count_ = 1;
	string give_skill_;
	//
	float life_ = 100.f;
	float maxlife_ = 100.f;
	float damage_ = 20.f;
	float defense_ = 5.f;
	float velocity_ = 1.f;
	float real_damage_ = 0.f; //真实伤害为负可以加血
	//以上都是数值，以下则是系数

protected:
	bool use_weapon_ = true;

private:
private:
	vector<Weapon *> wps_;
	vector<Ai *> ais_;
	vector<MultAi *> mais_;
	map<string, Skill *> skills_;
};
using ModuleInstance = Entity;

};

#endif
