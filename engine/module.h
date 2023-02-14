#ifndef MODULE_H
#define MODULE_H
#include "engine.h"
#include "bx2-engine.h"
#include "json/json.h"
namespace ns_module {
	using namespace std;
	using namespace ns_engine;
	using namespace ns_box2d;

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
protected:
private:
	mod_type mtype;
	bool is_center;
	generate_type gtype;
};

class ModuleFactory : public single<ModuleFactory> {
public:
	void LoadModules(const string &modpath);
	void LoadModule(const string &modpath);
	void UnLoadModules();
	Actor *Copy(const string &name, Layer* layer,const int& x,const int&y);
	void DestroyModule(Actor* mod) { delete mod;}

protected:
private:
	map<string, Actor *> mods;
};

};
#endif
