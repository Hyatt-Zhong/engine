#ifndef SKILL_H
#define SKILL_H
#include "menu.h"
#include "module.h"
#include "entity.h"
namespace ns_skill {
using namespace std;
using namespace ns_engine;
using namespace ns_box2d;
using namespace ns_module;
using namespace ns_menu;
using namespace ns_entity;
using namespace ns_module;

class Skill
{
public:
	Skill(const string &s, int cd, SDL_KeyCode k):shot(s),CD(cd),key(k) {}
	void Equip() {
		master->AddKeyEvent(key, [=](const SDL_Keycode &key, const Uint32 &up_or_down, void *data) {
			if (up_or_down == SDL_KEYDOWN) {
				auto actor = dynamic_cast<ModuleInstance *>(master);
				if (actor->FindSkill(this) && this->OverCD()) {
					int x, y;
					actor->GetSubGeneratePos(x, y);
					ModuleFactory::Instance()->SafeAddToLayer<ModuleInstance>(shot, master->parent_, master, x, y, "");
				}
			}
		});
	}
	void UnEquip() { master->RemoveKeyEvent(key);
	}
	void Update(const unsigned& dt) { overtime += dt;
	}
	void SetMaster(Actor *ma) { master = ma; }
	//void SetMenu(Layer *menu) {}
	Skill *Copy() { return new Skill(*this); }

protected:
	bool OverCD() {
		if (overtime >= CD) {
			overtime = 0;
			return true;
		}
		return false;
	}

protected:
	string shot;
	SDL_KeyCode key;
	int CD = 3000;//ms
	unsigned overtime = 3000;
	Actor *master = nullptr;

private:
};


};
#endif
