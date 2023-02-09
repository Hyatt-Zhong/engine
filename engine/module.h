#ifndef MODULE_H
#define MODULE_H
#include "engine.h"
#include "bx2-engine.h"
#include "json/json.h"
namespace ns_module {
	using namespace std;
	using namespace ns_engine;
	using namespace ns_box2d;

#define MTYPE_BEG(SP) enum SP {
#define MTYPE(TYPE) TYPE,
#define MTYPE_END };
		
#define MTYPE_MAP_BEG(SM) map<string, int> SM = {
#define MTYPE_MAP(TYPE) {#TYPE, TYPE},
#define MTYPE_MAP_END };

MTYPE_BEG(mod_type)
	MTYPE(building)
	MTYPE(enemy)
	MTYPE(npc)
	MTYPE(role)
	MTYPE(tree)
	MTYPE(weapon)
	MTYPE(bullet)
	MTYPE(skill)
	MTYPE(effect)
MTYPE_END



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

Actor *CreateMod(Game *wx, const string &asstpath,
	const string &strJsn, Layer *layer, bool isTest = false);

};
#endif
