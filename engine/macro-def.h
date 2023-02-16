#ifndef MACRO_DEF_H
#define MACRO_DEF_H

#define MTYPE_BEG(SP) enum SP {
#define MTYPE(TYPE) TYPE,
#define MTYPE_END };
		
#define MTYPE_MAP_BEG(KEY, VAL, SM) map<KEY, VAL> SM = {
#define MTYPE_MAP(TYPE) {#TYPE, TYPE},
#define MTYPE_MAPEX(KEY, VAL) { KEY,VAL},
#define MTYPE_MAP_END };

#define CHECK_POINT_VOID(p) if (!p) return;
#define CHECK_POINT_ZERO(p) if (!p) return 0;
#define CHECK_POINT_NULL(p) if (!p) return nullptr;

#define MAP_FIND(MAP,KEY,RET) \
auto it = MAP.find(KEY);     \
	if (it != MAP.end()) { \
		return it->second;   \
	}                            \
	return RET;

#define MAP_ERASE(MAP,KEY) \
auto it = MAP.find(KEY);     \
	if (it != MAP.end()) { \
		MAP.erase(it);   \
	}                           

#define JSON_VAL(JSN, KEY, TYPE, DEF) JSN.isMember(KEY) ? JSN[KEY].as##TYPE() : DEF;
#define MINUS1 -1.
#endif
