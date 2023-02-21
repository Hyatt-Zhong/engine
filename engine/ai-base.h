#ifndef AI_BASE_H
#define AI_BASE_H
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <memory>

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
	bool frame5() { return xcount % 5 == 1; }
	bool frame10() { return xcount % 10 == 1; }
	bool frame20() { return xcount % 20 == 1; }
	bool frame(const int &x) { return xcount % x == 1; }
	bool framex(const int &x) { return xcount % x == 0; }
	void frame() {
		xcount++;
		if (xcount >= xmaxcount) {
			xcount = 0;
		}
	}
	bool frame_with_count(const int &x) {//不必每帧都计算的，使用本函数跳过
		frame();
		return frame(x);
	}
	bool frame_with_count_ex(const int &x) {
		frame();
		return framex(x);
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
	virtual bool IsEnd() { return false; }
	virtual Ai *Copy() { return nullptr; }

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
	void RemoveMember(Actor *mem);
	void RemoveMember(); 
	void AddMember(Actor *actor, shared_ptr<void> data);
	map<Actor *, shared_ptr<void>> members_;

private:
};

using AiQuene = queue<pair<MultAi*,Actor*>>;
using AiContorl = map<string,MultAi*>;
};
#endif
