#ifndef MAP_H
#define MAP_H
#include "engine.h"
#include "bx2-engine.h"
#include "ai-base.h"

namespace ns_map {
using namespace std;
using namespace ns_engine;
using namespace ns_box2d;
using namespace ns_module;

class Map {
public:
	virtual void CreateOrUpdateActor() = 0;
	void SetLayer(Layer *ly) { layer = ly; }

	void AddToManage(Actor *actor);
	void Clear();

protected:
	Layer *layer = nullptr;
	vector<Actor *> actors;

private:
};

struct element {
	int x, y;
	string modname;
	string name;//索引名字，也是在内存中的名字
};

class CommonMap : public Map {
public:
	void Load(const string &file) {}
	void CreateOrUpdateActor() {}

private:
	void CreateActorBySequence() {}
	void CreateActorByPosition() {}

protected:
private:
};

class TestMap : public Map {
public:
	TestMap();
	void AddTestMod(const string &name, const string &modname, int x = 0, int y = 0);
	void CreateOrUpdateActor();

protected:
private:
	queue<element> que;
	vector<element> emts;

	int count = 0;
	int maxcount = 150;
};

class RogueMap:public Map {
public:
	void CreateOrUpdateActor() {}

protected:
private:
};
};
#endif
