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

const int kScoutRange = 10;

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

class LookAndMove : public Ai, public Scout {
public:
	bool Drive(Actor *actor);

protected:
private:
	Actor *point_ = nullptr;
};

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
	bool SetVel(Actor *actor, d_vel v, float length);
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

class MultCircleRole : public MultAi, public Scout {
public:
	bool Drive(Actor *actor);
	void SetClockwise(bool ck) { ck_ = ck; }

protected:
private:
	float distance_min_ = distance - distance_dt;
	float distance_max_ = distance + distance_dt;
	float dis_ = distance;

	int n_ = 24;
	bool ck_ = true;
};

class Line:public Ai
{
public:
	Line() {}
	Line(float xp) {
		que_.push(d_vel(xp, 0));
		que_.push(d_vel(0, xp));
		que_.push(d_vel(-xp, 0));
		que_.push(d_vel(0, -xp));
	}
	bool Drive(Actor *actor);
	void SetLine(const queue<d_vel> &que, bool loop = true) {
		que_ = que;
		loop_ = loop;
	}

	bool IsEnd() { return que_.empty(); }

protected:
	queue<d_vel> que_;
	bool loop_ = true;

	bool started_ = false;//是否设定了原点
	d_vel orgin_;//原点

private:
};

template<char T> 
class Patrol : public Line {
public:
	Patrol(float xp) {
		if (T == 'T') {
			que_.push(d_vel(-xp, 0));
			que_.push(d_vel(0, xp));
			que_.push(d_vel(xp, 0));
			que_.push(d_vel(0, -xp));
		} else {
			que_.push(d_vel(-xp, 0));
			que_.push(d_vel(0, -xp));
			que_.push(d_vel(xp, 0));
			que_.push(d_vel(0, xp));
		}
	}	

private:
};
using Clockwise = Patrol<'T'>;
using Counterclockwise = Patrol<'F'>;


Ai *AiMove();
Ai *AiCircle();
Ai *AiFollow();
Ai *AiCircleRole();
Ai *AiLook();
Ai *AiLine();
Ai *AiLookAndMove();
Ai *AiCounterclockwise();
Ai *AiGeneratLine();
MultAi *MAiMultCircleRole();
};
#endif
