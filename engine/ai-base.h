#ifndef AI_BASE_H
#define AI_BASE_H
#include <vector>
#include <string>
#include <map>
#include <queue>
namespace ns_engine {
class Actor;
class Layer;
}

namespace ns_ai {
using namespace std;
using namespace ns_engine;

class IntervalCount {
public:
	virtual ~IntervalCount() {}
	bool frame5() { return xcount % 5 == 0; }
	bool frame10() { return xcount % 10 == 0; }
	bool frame20() { return xcount % 20 == 0; }
	bool frame(const int &x) { return xcount % x == 0; }
	void frame() {
		xcount++;
		if (xcount >= xmaxcount) {
			xcount = 0;
		}
	}

protected:
private:
	int xcount = 0;
	int xmaxcount = 1001;
};

class Ai :public IntervalCount{
public:
	virtual bool Drive(Actor *actor) = 0; //返回true，执行完毕，激活下一个ai；返回false，继续执行当前ai
	void SetMaster(Actor *actor);
	bool Exist(Actor *actor);
	//virtual void *Data() { return nullptr; }
	//virtual string Name() = 0;

protected:
	Actor *master_ = nullptr;

private:
};

struct AiChain {
	vector<Ai*> chain;
	Ai *alive;
	unsigned index;

	AiChain() : alive(nullptr),index(0) {}
};

class MultAi : public Ai {
public:
protected:
	map<Actor *, void *> members_;

private:
};

using AiQuene = queue<MultAi*>;
using AiContorl = map<string,MultAi*>;
};
#endif
