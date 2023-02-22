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
	Skill(const string nm,const string &s, int cd, SDL_KeyCode k,const string& path):name(nm),shot(s),CD(cd),key(k),ico(path) {}
	void Equip() {
		auto mn = Game::Instance()->GetLayer("mn_skill");
		dynamic_cast<SkillMenu *>(mn)->PushSkill(name,ico);
		master->AddKeyEvent(key, [=](const SDL_Keycode &key, const Uint32 &up_or_down, void *data) {
			if (up_or_down == SDL_KEYDOWN) {
				auto actor = dynamic_cast<ModuleInstance *>(master);
				if (actor->FindSkill(name) && this->OverCD()) {
					int x, y;
					actor->GetSubGeneratePos(x, y);
					ModuleFactory::Instance()->SafeAddToLayer<ModuleInstance>(shot, master->parent_, master, x, y, "");
				}

			}
		});
	}
	void UnEquip() { master->RemoveKeyEvent(key);
		auto mn = Game::Instance()->GetLayer("mn_skill");
		dynamic_cast<SkillMenu *>(mn)->RemoveSkill(name);
	}
	void Update(const unsigned& dt) { overtime += dt;
		if (overtime < CD) {
			float x = overtime / (float)CD;
			Game::Instance()->OnNotice("mn_skill", name, &x);
		}
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
	string name;
	string shot;
	SDL_KeyCode key;
	int CD = 3000;//ms
	string ico;
	unsigned overtime = 3000;
	Actor *master = nullptr;

private:
};


};
#endif
