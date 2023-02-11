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

Actor *CreateMod(Game *wx, const string &asstpath,
	const string &strJsn, Layer *layer, bool isTest = false);

};
#endif
