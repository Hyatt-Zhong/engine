#include "ai.h"

const float kPI = 3.1415926f;
const float kAngle = 57.295779f;
using namespace ns_engine;
namespace ns_ai {
bool Scout::ReportFixture(b2Fixture *fixture) {
	auto body = fixture->GetBody();
	auto actor = (Actor *)body->GetUserData().pointer;
	if (actor->type_ == goal_type_) {
		result_ = actor;
		return false;
	}
	return true;
}
Actor *Scout::Search(const int &range, Actor *point, int type) {
	auto b2_pos = point->world_->GetBody(point)->GetPosition();
	auto x = b2_pos.x;
	auto y = b2_pos.y;

	b2AABB aabb;
	aabb.lowerBound.Set(x - range, y - range);
	aabb.upperBound.Set(x + range, y + range);

	result_ = nullptr;
	goal_type_ = type;
	point->world_->World()->QueryAABB(this, aabb);
	return result_;
}

bool Follow::Drive(Actor *actor) {
	if (point == nullptr) {
		point = Search(10, actor, actor->goaltype_);
	}
	if (point) {
		d_vel v(point->x_ - actor->x_, point->y_ - actor->y_);
		//print(v.x, v.y);
		v.Normalize();
		d_vel uv(0, 1);
		auto cosangle = b2Dot(uv, v) / (v.Length() * uv.Length());
		auto angle = acosf(cosangle) * kAngle;
		actor->angle_ = v.x >= 0 ? angle : -angle;
		//print(cosangle, actor->angle_);
		v *= 1.f;
		actor->SetVel(v);
	}
	return false;
}



};
