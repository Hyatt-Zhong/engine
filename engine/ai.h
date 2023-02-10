#ifndef AI_H
#define AI_H
#include "engine.h"
#include "bx2-engine.h"
#include "ai-base.h"

namespace ns_ai {
using namespace std;
using namespace ns_engine;
using namespace ns_box2d;

//#define AI_NAME(NAME) string name = NAME; string Name() { return NAME; }

class Scout : public b2QueryCallback {
public:
	bool ReportFixture(b2Fixture *fixture);
	Actor *Search(const int &range, Actor *point, int type);

protected:
private:
	Actor *result_;
	int goal_type_;
};

class Follow : public Ai, public Scout {
public:
	bool Drive(Actor *actor);
	//AI_NAME("Follow")
protected:
private:
	Actor *point = nullptr;
};

template<char T>
class Move:public Ai
{
public:
	bool Drive(Actor* actor) { 
		switch (T) {
		case 'L':
			actor->SetVel(d_vel(-xv, 0));
			break;
		case 'R':
			actor->SetVel(d_vel(xv, 0));
			break;
		case 'U':
			actor->SetVel(d_vel(0, xv));
			break;
		case 'D':
			actor->SetVel(d_vel(0, -xv));
			break;
		default:
			break;
		}
		count++;
		if (count >= maxcount) {
			count = 0;
			return true;
		}
		return false;
	}
	//AI_NAME(string("Move")+T)
protected:
	const float xv = 1;

private:
	int count = 0;
	const int maxcount = 90;
};

using MoveLeft = Move<'L'>;
using MoveRight = Move<'R'>;
using MoveUp = Move<'U'>;
using MoveDown = Move<'D'>;

};
#endif
