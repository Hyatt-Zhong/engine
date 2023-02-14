#include "ai-base.h"
#include "ai.h"
#include "engine.h"

const float kPI = 3.1415926f;
const float k2PI = 6.2831852f;
const float kAngle = 57.295779f;
using namespace ns_engine;
namespace ns_ai {

void Ai::SetLayer(Layer *layer) {
	layer_ = layer;
}
bool Ai::Exist(Actor *actor) {
	return layer_->Exist(actor);
}

bool Scout::ReportFixture(b2Fixture *fixture) {
	auto body = fixture->GetBody();
	auto actor = (Actor *)body->GetUserData().pointer;
	if (search_one_)
	{
		if (goal_type_.test(actor->type_)) {
			result_ = actor;
			return false;
		}
	} else {
		if (goal_type_.test(actor->type_)) {
			results_.push_back(actor);
			return results_.size() >= search_count;
		}
	}
	return true;
}
Actor *Scout::Search(const int &range, Actor *point, typeset type) {
	auto b2_pos = point->world_->GetBody(point)->GetPosition();
	auto x = b2_pos.x;
	auto y = b2_pos.y;

	b2AABB aabb;
	aabb.lowerBound.Set(x - range, y - range);
	aabb.upperBound.Set(x + range, y + range);

	result_ = nullptr;
	goal_type_ = type;
	search_one_ = true;
	point->world_->World()->QueryAABB(this, aabb);
	return result_;
}

vector<Actor*> Scout::SearchMult(const int& range, Actor* point, typeset type, int count /*= 4*/) {
	auto b2_pos = point->world_->GetBody(point)->GetPosition();
	auto x = b2_pos.x;
	auto y = b2_pos.y;

	b2AABB aabb;
	aabb.lowerBound.Set(x - range, y - range);
	aabb.upperBound.Set(x + range, y + range);

	result_ = nullptr;
	goal_type_ = type;
	search_one_ = false;
	search_count = count;
	results_.clear();
	point->world_->World()->QueryAABB(this, aabb);
	return results_;
	}


bool Follow::Drive(Actor *actor) {
	if (!point && !Exist(point)) {
		point = Search(range_, actor, actor->goaltype_);
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
	} else {
		return true;
	}
	return false;
}

bool Circle::Drive(Actor *actor) {
	if (!point && !Exist(point)) {
		point = Search(range_, actor, actor->goaltype_);
	}
	if (point) {
		d_vel v(point->x_ - actor->x_, point->y_ - actor->y_);
		//print(v.x, v.y);
		auto length = v.Normalize();
		if (length > distance_max_) {
			v *= velocity_big;
			actor->SetVel(v);
			print("big");
		} else if (length < distance_min_) {
			v *= velocity_big;
			actor->SetVel(-v);
			print("small");
		} else if (length <= distance_max_ && length >= distance_min_) {
			d_vel v1(point->x_, point->y_);
			b2Rot rt(-k2PI / n_);
			if (!ck_) {
				rt.Set(k2PI / n_);
			}
			auto vv = b2Mul(rt, v);
			vv *= dis_;
			v1 -= vv;
			d_vel xv(v1.x - actor->x_, v1.y - actor->y_);
			xv.Normalize();
			xv *= velocity_small;
			actor->SetVel(xv);
		}
		//frame();
		//if (frame5()) {
			/*d_vel uv(0, 1);
			auto cosangle = b2Dot(uv, v) / (v.Length() * uv.Length());
			auto angle = acosf(cosangle) * kAngle;
			actor->angle_ = v.x >= 0 ? angle : -angle;*/
		//}
	} else {
		return true;
	}
	return true;
}


};
