#ifndef AI_H
#define AI_H
#include "engine.h"
#include "bx2-engine.h"
#include "ai-base.h"

namespace ns_weapon {
class Weapon;
};

namespace ns_ai {
using namespace std;
using namespace ns_engine;
using namespace ns_box2d;
using namespace ns_module;
using namespace ns_weapon;

//#define AI_NAME(NAME) string name = NAME; string Name() { return NAME; }

const int kScoutRange = 5;

class Scout : public b2QueryCallback {
public:
	bool ReportFixture(b2Fixture *fixture);
	Actor *Search(const int &range, Actor *point, typeset type);
	vector<Actor *> SearchMult(const int &range, Actor *point, typeset type, int count = 4);
	void SetRange(int range) { range_ = range; }

protected:
	int range_ = kScoutRange;

private:
	Actor *result_;
	typeset goal_type_;
	bool search_one_ = true;
	int search_count = 4;
	vector<Actor *> results_;
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


class Move:public Ai
{
public:
	Move() :direct_(0.,1.){}
	Move(const d_vel& direct) : direct_(direct) {}
	void SetDirect(const d_vel &direct) { direct_ = (direct); }
	bool Drive(Actor *actor);
	//AI_NAME(string("Move")+T)
protected:
	d_vel direct_;

private:
};

class Look : public Ai, public Scout {
public:
	bool Drive(Actor *actor);

protected:
private:
	Actor *point_ = nullptr;
};

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

const static float distance = 123.f;
const static float distance_dt = 30.f;
const static float velocity_big = 6.f;
const static float velocity_small = 3.f;
class Circle : public Ai, public Scout {
public:
	Circle(int n, float dis) {
		distance_min_ = dis - distance_dt;
		distance_max_ = dis + distance_dt;
		n_ = n;
		dis_ = dis;
	}
	Circle(int n) {
		n_ = n;
	}
	Circle(float dis) {
		distance_min_ = dis - distance_dt;
		distance_max_ = dis + distance_dt;
		dis_ = dis;
	}
	Circle() {}
	bool Drive(Actor *actor);
	void SetPoint(Actor *point) { point_ = point; }
	void SetClockwise(bool ck) { ck_ = ck; }
	//AI_NAME("Follow")
protected:
private:
	Actor *point_ = nullptr;
	int n_ = 24;
	float distance_min_ = distance - distance_dt;
	float distance_max_ = distance + distance_dt;
	float dis_ = distance;
	bool ck_ = true;
};

class CircleRole : public Circle {
public:
	bool Drive(Actor *actor);

protected:
private:
};

class Line
{
public:
	Line() {}

protected:
private:
};

Ai *AiMoveUp();
Ai *AiCircle();
Ai *AiFollow();
Ai *AiCircleRole();
Ai *AiLook();
};
#endif
