#ifndef AI_H
#define AI_H
#include "engine.h"
#include "bx2-engine.h"
#include "ai-base.h"

namespace ns_ai {
using namespace std;
using namespace ns_engine;
using namespace ns_box2d;
using namespace ns_module;

//#define AI_NAME(NAME) string name = NAME; string Name() { return NAME; }

const int kScoutRange = 5;

class Scout : public b2QueryCallback {
public:
	bool ReportFixture(b2Fixture *fixture);
	Actor *Search(const int &range, Actor *point, typeset type);
	void SetRange(int range) { range_ = range; }

protected:
	int range_ = kScoutRange;

private:
	Actor *result_;
	typeset goal_type_;
	bool search_one_ = true;
};

class Follow : public Ai, public Scout {
public:
	bool Drive(Actor *actor);
	void SetPoint(Actor *point_) { point = point_; }
	//AI_NAME("Follow")
protected:
private:
	Actor *point = nullptr;
};

template<char T>
class Move:public Ai
{
public:
	Move() {}
	Move(const float& v, const int& max_count) : xv(v), maxcount(max_count) {}
	Move(const float& v) : xv(v) {}
	Move(const int& max_count) : maxcount(max_count) {}
	bool Drive(Actor *actor) { 
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

template<char T> 
class Patrol : public Ai, public Scout {
public:
	Patrol() { SetDirect(); }
	Patrol(const float &v, const int &max_count) : xv(v), maxcount(max_count) { SetDirect(); }
	Patrol(const float &v) : xv(v) { SetDirect(); }
	Patrol(const int &max_count) : maxcount(max_count) { SetDirect(); }

	bool Drive(Actor *actor) {
		switch (direct) {
		case LD:
			actor->SetVel(d_vel(-xv, -xv));
			break;
		case LU:
			actor->SetVel(d_vel(-xv, xv));
			break;
		case RU:
			actor->SetVel(d_vel(xv, xv));
			break;
		case RD:
			actor->SetVel(d_vel(xv, -xv));
			break;
		default:
			break;
		}
		SwitchDirect();
		if (count % dt == 1) {
			auto point = Search(range_, actor, actor->goaltype_);
			if (point) {
				return true;
			}
		}
		return false;
	}

	void SetDt(int dt_) { dt = dt_;	}

private:
	void SetDirect() {
		if (T == 'T') {
			ls_direct = {LD, LU, RU, RD};
		} else {
			ls_direct = {LU, LD, RD, RU};
		}
		direct = ls_direct[0];
	}
	void SwitchDirect() {
		count++;
		if (count >= maxcount) {
			count = 0;
		} else {
			return;
		}
		index++;
		if (index >= ls_direct.size()) {
			index = 0;
		}
		direct = ls_direct[index];
	}
	//AI_NAME(string("Move")+T)
protected:
	const float xv = 1;

	enum xdirect { NONE = 0, LD = 1, LU, RU, RD };
	xdirect direct = NONE;
	vector<xdirect> ls_direct;
	int index = 0;

private:
	int dt = 5;
	int count = 0;
	const int maxcount = 90;
};
using Clockwise = Patrol<'T'>;
using Counterclockwise = Patrol<'F'>;
};
#endif
