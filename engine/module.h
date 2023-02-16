#ifndef MODULE_H
#define MODULE_H
#include "engine.h"
#include "bx2-engine.h"
#include "json/json.h"

namespace ns_weapon {
class Weapon;
};

namespace ns_module {
	using namespace std;
	using namespace ns_engine;
	using namespace ns_box2d;
	using namespace ns_weapon;

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

protected:
	vector<string> ai_names_;
	vector<string> wp_names_;

private:
	bool is_center_;
	generate_type gtype_;
};

extern map<string, Module::CreateAi> kAiMap;
extern map<string, Module::CreateWeapon> kWeaponMap;


class ModuleFactory : public single<ModuleFactory> {
public:
	void LoadModules(const string &modpath);
	template<typename T> void LoadModule(const string &strJsn) {
		Json::Value jsn;
		if (!ParseJson(strJsn, jsn)) {
			return;
		}
		auto name = jsn["modname"].asString();

		auto mod = new T;
		mod->SetParam(jsn);

		mods[name] = mod;
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
	void SafeAddToLayer(const string &modname, Layer *layer, Actor* master, const int &x, const int &y, const string &name) {
		layer->AddFrameEventOnce([=](void *) {
			auto bullet = ModuleFactory::Instance()->Copy<T>(modname, (Layer *)layer, x, y, name);
			bullet->goaltype_ = master->goaltype_;
			layer->GetMap()->AddToManage(bullet);
		});
	}
	void DestroyModule(Layer *layer, Actor *mod);

private:
	bool ParseJson(const string &strJsn, Json::Value &jsn);
	void AddToLayer(Module *mod, Layer *layer, const int &x, const int &y, const string &name);

protected:
private:
	map<string, Actor *> mods;
};

};
#endif
