#include "ai-base.h"
#include "ai.h"
#include "engine.h"
#include "weapon.h"
#include "entity.h"

const float kPI = 3.1415926f;
const float k2PI = 6.2831852f;
const float kAngle = 57.295779f;
using namespace ns_engine;
using namespace ns_entity;

namespace ns_ai {

void SetAngle(Actor* actor, const d_vel& v) {
	d_vel uv(0, 1);
	auto cosangle = b2Dot(uv, v) / (v.Length() * uv.Length());
	auto angle = acosf(cosangle) * kAngle;
	actor->angle_ = v.x >= 0 ? angle : -angle;
}

void Ai::SetMaster(Actor *actor) {
	master_ = actor;
}
bool Ai::Exist(Actor *actor) {
	return master_->parent_->Exist(actor);
}

bool Scout::ReportFixture(b2Fixture *fixture) {
	auto body = fixture->GetBody();
	auto actor = (Actor *)body->GetUserData().pointer;
	if (search_one_) {
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
	auto phybody = point->world_->GetBody(point);
	//CHECK_POINT_NULL(phybody)
	auto b2_pos = phybody->GetPosition();
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
	if (!Exist(point)) {
		point = Search(range_, actor, actor->goaltype_);
	}
	if (point) {
		d_vel v(point->x_ - actor->x_, point->y_ - actor->y_);
		//print(v.x, v.y);
		v.Normalize();
		actor->SetDirect(v);
		SetAngle(actor, v);
		//print(cosangle, actor->angle_);
		v *= dynamic_cast<ModuleInstance *>(master_)->velocity_;
		actor->SetVel(v);
	} else {
		return true;
	}
	return false;
}

bool Circle::Drive(Actor *actor) {
	if (!Exist(point_)) {
		point_ = Search(range_, actor, actor->goaltype_);
	}
	if (point_) {
		dynamic_cast<ModuleInstance *>(actor)->UseWeapon(true);
		d_vel v(point_->x_ - actor->x_, point_->y_ - actor->y_);
		//print(v.x, v.y);
		auto length = v.Normalize();
		actor->SetDirect(v);
		if (length > distance_max_) {
			v *= velocity_big;
			actor->SetVel(v);
			print("big");
		} else if (length < distance_min_) {
			v *= velocity_big;
			actor->SetVel(-v);
			print("small");
		} else if (length <= distance_max_ && length >= distance_min_) {
			d_vel v1(point_->x_, point_->y_);
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
			SetAngle(actor, v);
		//}
	} else {
		return true;//找不到就结束
	}
	return false;
}


Ai *AiMoveUp() {
	auto ai = new Move;
	return ai;
}

Ai *AiCircle() {
	return new Circle;
}

Ai *AiFollow() {
	return new Follow;
}

Ai *AiCircleRole() {
	return new CircleRole;
}

Ai *AiLook() {
	return new Look;
}

bool Move::Drive(Actor *actor) {
	auto v = direct_;
	v *= dynamic_cast<ModuleInstance *>(master_)->velocity_;
	actor->SetVel(v);
	return true;
}

bool Look::Drive(Actor *actor) {
	if (!Exist(point_)) {
		point_ = Search(range_, actor, actor->goaltype_);
	}
	if (point_) {
		dynamic_cast<ModuleInstance *>(actor)->UseWeapon(true);
		d_vel v(point_->x_ - actor->x_, point_->y_ - actor->y_);
		//print(v.x, v.y);
		auto length = v.Normalize();
		actor->SetDirect(v);
		SetAngle(actor, v);
	} else {
		dynamic_cast<ModuleInstance *>(actor)->UseWeapon(false);
	}
	return true;
}

bool CircleRole::Drive(Actor *actor) {
	auto point = Search(range_, actor, actor->goaltype_);
	if (point) {
		return true;
	} else {
		SetPoint(Game::Instance()->Leadrol());
		Circle::Drive(actor);
		dynamic_cast<ModuleInstance *>(actor)->UseWeapon(false);//必须在这行设置为false,因为Circle::Drive会设置为true
	}
	return false;
}

};
