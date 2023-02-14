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

protected:
	Layer *layer = nullptr;

private:
};

struct element {
	int x, y;
	string name;
};

class CommonMap
{
public:
	void Load(const string &file) {}
	void CreateOrUpdateActor() {}

private:
	void CreateActorBySequence() {}
	void CreateActorByPosition() {}

protected:
private:
};

class TestMap:public Map
{
public:
	TestMap();
	void AddTestMod(const string &name);
	void CreateOrUpdateActor();

protected:
private:
	queue<element> que;
};

class RogueMap:public Map {
public:
	void CreateOrUpdateActor() {}

protected:
private:
};
};
#endif
