#ifndef MAP_H
#define MAP_H
#include "engine.h"
#include "bx2-engine.h"
#include "ai-base.h"
#include "utily.h"

namespace ns_combination {
class Combination;
}

namespace ns_map {
using namespace std;
using namespace ns_engine;
using namespace ns_box2d;
using namespace ns_module;
using namespace ns_utily;
using namespace ns_combination;

using RoleCreateEvent = void (*)(void *);
class Map {
public:
	virtual ~Map() {}
	virtual void CreateOrUpdateActor() = 0;
	void SetRoleCreateEvent(RoleCreateEvent ce) { ce_ = ce; }
	void SetLayer(Layer *ly) { layer = ly; }

	void AddToManage(Actor *actor);
	void Clear();
	virtual Map *Copy() = 0;

protected:
	Layer *layer = nullptr;
	vector<Actor *> actors;
	RoleCreateEvent ce_;

private:
};

class CommonMap : public Map {
public:
	struct ComData {
		float x, y;
		string comname;
	};
	struct RoleData {
		float x, y;
		string name;
		int life;
	};
	struct ExData {
		float x, y;
		string name;
	};

public:
	void Load(const string &strMap);
	string GetName();

	void CreateOrUpdateActor();
	bool IsEnd();
	Map *Copy() { return new CommonMap(*this); }

public:
	void AddCombo(Combination *combo) { cur_combos_.push_back(combo); }

private:
	void CreateActorBySequence() {}
	void CreateActorByPosition() {}

protected:
	queue<pair<queue<ComData>,bool>> que_;
	string map_name_;
	string bgm_;
	vector<Combination *> cur_combos_;
	bool cur_need_clear = false;

	RoleData rdata_;

	vector<ExData> exdata_;

private:
};

using MapInstance = CommonMap;

class MapManager : public single<MapManager> {
public:
	~MapManager() {
		for (auto &it : maps) {
			SAFE_DELETE(it.second);
		}
	}
	Map *CopyMap(const string &name) { 
		auto it = maps.find(name);
		if (it != maps.end()) {
			return it->second->Copy();
		}
		return nullptr;
	}
	void ReleaseMap(Map *map) { SAFE_DELETE(map);
	}
	void LoadMaps(const string &path);
	void UnLoadMaps();

protected:
private:
	map<string, Map *> maps;
};

class RogueMap:public Map {
public:
	void CreateOrUpdateActor() {}

protected:
private:
};

struct element {
	int x, y;
	string modname;
	string name; //索引名字，也是在内存中的名字
};

class TestMap : public Map {
public:
	TestMap();
	void AddTestMod(const string &name, const string &memname, int x = 0, int y = 0);
	void CreateOrUpdateActor();
	Map *Copy() { return new TestMap(*this); }

protected:
private:
	queue<element> que;
	vector<element> emts;

	int count = 0;
	int maxcount = 150;
};
};
#endif
