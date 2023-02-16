#ifndef WEAPON_H
#define WEAPON_H
#include "engine.h"
#include "bx2-engine.h"
#include "ai-base.h"

namespace ns_weapon {
using namespace std;
using namespace ns_engine;
using namespace ns_box2d;

class Weapon : public IntervalCount {
public:
	virtual void Use() = 0;
	void SetMaster(Actor *master);

protected:
	Layer *layer_ = nullptr;
	Actor *master_ = nullptr;

private:
};

class Alpha :public Weapon{
public:
	Alpha();
	Alpha(const string &bullet);
	void Use();
	void SetBullet(const string &bullet);

protected:
private:
	d_vel direct_;
	string bullet_;

};

Weapon *WPAlpha();
Weapon *WPFollowBullet();

};
#endif
