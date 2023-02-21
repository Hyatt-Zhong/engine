#ifndef MODULE_H
#define MODULE_H
#include "engine.h"
#include "bx2-engine.h"
#include "json/json.h"
#include "combination.h"
#include "utily.h"
#include "menu.h"
#include "map.h"

namespace ns_weapon {
class Weapon;
};

namespace ns_module {
	using namespace std;
	using namespace ns_engine;
	using namespace ns_box2d;
	using namespace ns_weapon;
	using namespace ns_combination;
	using namespace ns_utily;
	using namespace ns_map;
	using namespace ns_menu;


enum generate_type {
	common,
	hide_in_other,//藏在某处，然后跳出来
	from_screen_else,
};

MTYPE_BEG(mod_direct)
MTYPE(idle)
MTYPE(idle_up)
MTYPE(idle_down)
MTYPE(idle_left)
MTYPE(idle_right)
MTYPE(up)
MTYPE(down)
MTYPE(left)
MTYPE(right)
MTYPE_END


class ModuleFactory;
class Module : public Actor {
	friend class ModuleFactory;
public:
	virtual void SetParam(const Json::Value &jsn);
	virtual void Init(); //我拷贝module时都是用浅拷贝，所以需额外深拷贝时都用init函数

	using CreateAi = Ai *(*)();
	using CreateWeapon = Weapon *(*)();
	using CreateMultAi = MultAi *(*)();

protected:
	vector<string> ai_names_;
	vector<string> wp_names_;
	vector<string> mai_names_;

private:
	bool is_center_;
	generate_type gtype_;
};

extern map<string, Module::CreateAi> kAiMap;
extern map<string, Ai*> kExAiMap;
extern map<string, Module::CreateMultAi> kMAiMap;
extern map<string, Module::CreateWeapon> kWeaponMap;

extern map<string, int> kModType;
extern const string kstrGoalType;
extern const string kstrAi;

class ModuleFactory : public single<ModuleFactory> {
public:
	void LoadModules(const string &modpath);
	void LoadAi(const string &strJsn);
	template<typename T> 
	void LoadModule(const string &strJsn) {
		Json::Value jsn;
		if (!ParseJson(strJsn, jsn)) {
			return;
		}
		auto name = jsn["modname"].asString();

		auto mod = new T;
		mod->SetParam(jsn);

		mods[name] = mod;
	}

	template<typename T> 
	void LoadCombination(const string &strJsn) {
		Json::Value jsn;
		if (!ParseJson(strJsn, jsn)) {
			return;
		}
		auto name = jsn["modname"].asString();

		auto com = new T;
		com->SetParam(jsn);

		combis[name] = com;
	}

	void UnLoadModules();

	template<typename T> 
	Actor *Copy(const string &modname, Layer *layer, const int &x, const int &y, const string &name = "") {
		auto mod = new T(*(T *)mods[modname]);
		mod->Init();
		AddToLayer(mod, layer, x, y, name);
		return mod;
	}

	template<typename T> 
	Actor *CopyCombi(const string &modname, Layer *layer, const int &x, const int &y) {
		auto com = new T(*(T *)combis[modname]);
		com->Init();
		AddToLayer(com, layer, x, y);
		return com;
	}

	template<typename T> void SafeAddToLayer(const string &modname, Layer *layer, const int &x, const int &y, d_vel direct) {
		layer->AddFrameEventOnce([=](void *) {
			auto mod = ModuleFactory::Instance()->Copy<T>(modname, (Layer *)layer, x, y, "");
			mod->direct_ = direct;
			layer->GetMap()->AddToManage(mod);
		});
	}

	template<typename T>
	void SafeAddToLayer(const string &modname, Layer *layer, Actor* master, const int &x, const int &y, const string &name) {
		//如果下一帧master被销毁了，goaltype和direct都无法访问了，所以先记录下来
		auto goaltype = master->goaltype_;
		auto direct = master->direct_;
		layer->AddFrameEventOnce([=](void *) {
			auto bullet = ModuleFactory::Instance()->Copy<T>(modname, (Layer *)layer, x, y, name);
			bullet->goaltype_ = goaltype;
			bullet->direct_ = direct;
			layer->GetMap()->AddToManage(bullet);
		});
	}

	using AfterCreateCombo = void (*)(void *, void *);
	template<typename T>
	void SafeAddCombinationToLayer(const string &modname, Layer *layer, const int &x, const int &y, Map *map = nullptr,
				       AfterCreateCombo fn = nullptr, Actor *master = nullptr) {
		layer->AddFrameEventOnce([=](void *) {
			auto combi = ModuleFactory::Instance()->CopyCombi<T>(modname, (Layer *)layer, x, y);

			if (master && !master->IsDeath()) {
				combi->goaltype_ = master->goaltype_;
				combi->direct_ = master->direct_;
			}
			if (fn) {
				fn(map, combi);
			}
			layer->GetMap()->AddToManage(combi);
		});
	}

	using AfterCreate = void(*)(void *,void *);
	template<typename T> 
	void SafeAddToLayerByCombination(const string &modname, Layer *layer, Combination /*用Actor是错误的*/ *combi, const int &x, const int &y,AfterCreate fn=nullptr) {
		//combination是一个玩家无法感知的东西，而且它一定比它生成的成员后死，所以不用记录goaltype和direct
		//auto goaltype = combi->goaltype_;
		//auto direct = combi->direct_;
		layer->AddFrameEventOnce([=](void *) {
			auto actor = ModuleFactory::Instance()->Copy<T>(modname, (Layer *)layer, x, y);
			if (combi->goaltype_ != 0) {
				actor->goaltype_ = combi->goaltype_;
			}
			actor->direct_ = combi->direct_;
			
			actor->PushAi(dynamic_cast<MultAi*>(combi), combi);//这里必须用动态转换，且不能用Actor转MultAi，因为它们没有继承关系
			if (fn) {
				fn(combi, actor);
			}
			layer->GetMap()->AddToManage(actor);
			if (actor->type_.test(boss)) {
				actor->AddFrameEvent([](void *self) {
					auto data = new ns_menu::Blood::BloodData;
					auto rolex = (ModuleInstance *)self;
					data->val = rolex->life_;
					data->max = rolex->maxlife_;
					Game::Instance()->OnNotice("info", "bd_boss", data);
				});
			}
		});
	}
	void DestroyModule(Layer *layer, Actor *mod);

private:
	void AddToLayer(Module *mod, Layer *layer, const int &x, const int &y, const string &name);
	void AddToLayer(Combination *mod, Layer *layer, const int &x, const int &y);

protected:
private:
	map<string, Actor *> mods;
	map<string, Actor *> combis;
};

};
#endif
