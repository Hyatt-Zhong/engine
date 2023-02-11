#ifndef AI_BASE_H
#define AI_BASE_H

namespace ns_engine {
class Actor;
}

namespace ns_ai {
using namespace std;
using namespace ns_engine;

class Ai {
public:
	virtual bool Drive(Actor *actor) = 0; //����true��ִ����ϣ�������һ��ai
	//virtual void *Data() { return nullptr; }
	//virtual string Name() = 0;

protected:
private:
};
struct AiChain {
	vector<Ai *> chain;
	Ai *alive;
	unsigned index;

	AiChain() : alive(nullptr),index(0) {}
};

};
#endif
