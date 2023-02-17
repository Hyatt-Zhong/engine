#include "ai-base.h"
#include "ai.h"
#include "engine.h"
#include "weapon.h"
#include "entity.h"

const float kPI = 3.1415926f;
const float k2PI = 6.2831852f;
const float kPI_2 = 1.5707963f;
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
	//return master_->parent_->Exist(actor);
	return actor&&!actor->IsDeath();
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
		d_vel v(point->GetCenter().x - actor->GetCenter().x, point->GetCenter().y - actor->GetCenter().y);
		//print(v.x, v.y);
		v.Normalize();
		actor->SetDirect(v);
		SetAngle(actor, v);
		//print(cosangle, actor->angle_);
		v *= dynamic_cast<ModuleInstance *>(actor)->velocity_;
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
		//dynamic_cast<ModuleInstance *>(actor)->UseWeapon(true);
		d_vel v(point_->GetCenter().x - actor->GetCenter().x, point_->GetCenter().y - actor->GetCenter().y);
		//print(v.x, v.y);
		auto length = v.Normalize();
		actor->SetDirect(v);
		SetVel(actor, v, length);
		//frame();
		//if (frame5()) {
			SetAngle(actor, v);
		//}
	} else {
		return true;//找不到就结束
	}
	return false;
}

bool Circle::SetVel(Actor *actor, d_vel v, float length) {
	if (length > distance_max_) {
		v *= velocity_big;
		actor->SetVel(v);
		return false;
	} else if (length < distance_min_) {
		v *= velocity_big;
		actor->SetVel(-v);
		return false;
	} else if (length <= distance_max_ && length >= distance_min_) {
		d_vel v1(point_->GetCenter().x, point_->GetCenter().y);
		b2Rot rt(-k2PI / n_);
		if (!ck_) {
			rt.Set(k2PI / n_);
		}
		auto vv = b2Mul(rt, v);
		vv *= dis_;
		v1 -= vv;
		d_vel xv(v1.x - actor->GetCenter().x, v1.y - actor->GetCenter().y);
		xv.Normalize();
		xv *= velocity_small;
		actor->SetVel(xv);
		return true;
	}
	return false;
}



bool Move::Drive(Actor *actor) {
	auto v = dynamic_cast<ModuleInstance *>(actor)->direct_;
	SetAngle(actor, v);
	v *= dynamic_cast<ModuleInstance *>(actor)->velocity_;
	actor->SetVel(v);
	return true;
}

bool Look::Drive(Actor *actor) {
	if (!Exist(point_)) {
		point_ = Search(range_, actor, actor->goaltype_);
	}
	if (point_) {
		dynamic_cast<ModuleInstance *>(actor)->UseWeapon(true);
		d_vel v(point_->GetCenter().x - actor->GetCenter().x, point_->GetCenter().y - actor->GetCenter().y);
		//print(v.x, v.y);
		auto length = v.Normalize();
		actor->SetDirect(v);
		SetAngle(actor, v);
		actor->SetVel(d_vel(0,0));
		return false;
	} else {
		dynamic_cast<ModuleInstance *>(actor)->UseWeapon(false);
	}
	return true;
}

bool LookAndMove::Drive(Actor *actor) {
	if (!Exist(point_)) {
		point_ = Search(range_, actor, actor->goaltype_);
	}
	if (point_) {
		dynamic_cast<ModuleInstance *>(actor)->UseWeapon(true);
		d_vel v(point_->GetCenter().x - actor->GetCenter().x, point_->GetCenter().y - actor->GetCenter().y);
		//print(v.x, v.y);
		auto length = v.Normalize();
		actor->SetDirect(v);
		SetAngle(actor, v);
		auto xv = actor->GetVel();
		if (dynamic_cast<ModuleInstance *>(actor)->velocity_ - xv.Length() <= .5f) {
			xv.x /= 3;
			xv.y /= 3;
		}
		actor->SetVel(xv);
		return false;
	} else {
		dynamic_cast<ModuleInstance *>(actor)->UseWeapon(false);
	}
	return true;
}

bool CircleRole::Drive(Actor *actor) {
	auto point = Search(range_, actor, actor->goaltype_);
	if (point) {
		dynamic_cast<ModuleInstance *>(actor)->UseWeapon(true);
		return true;
	} else {
		point = Game::Instance()->Leadrol();
		if (!point) {
			return true;
		}
		SetPoint(point);

		d_vel v(point->GetCenter().x - actor->GetCenter().x, point->GetCenter().y - actor->GetCenter().y);
		auto length = v.Normalize();
		auto up = SetVel(actor, v, length);
		if (up) {
			auto uv = d_vel(0, 1);
			actor->SetDirect(uv);
			SetAngle(actor, uv);
			dynamic_cast<ModuleInstance *>(actor)->UseWeapon(true);
			if (Exist(point)) {
				auto mai = point->FindContorlAi("MAiMultCircleRole");
				actor->PushAi(mai);
				return true;
			}
		} else {
			actor->SetDirect(v);
			SetAngle(actor, v);
			dynamic_cast<ModuleInstance *>(actor)->UseWeapon(false);
		}
	}
	return false;
}

bool Line::Drive(Actor *actor) {
	d_vel pos = actor->GetCenterB2();
	if (!started_) {
		orgin_ = pos;
		started_ = true;
	}
	auto xv = que_.front();
	auto recode = xv;
	xv += orgin_;
	xv -= pos;
	auto length = xv.Normalize();

	actor->SetDirect(xv);
	SetAngle(actor, xv);
	xv *= dynamic_cast<ModuleInstance *>(actor)->velocity_;
	actor->SetVel(xv);

	if (length < 5) {
		que_.pop();
		if (loop_) {
			que_.push(recode);
		}
	}
	return true;
}



Ai *AiMove() {
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

Ai* AiLine() {
	return new Line(200);
}

Ai *AiLookAndMove() {
	return new LookAndMove;
}

Ai *AiCounterclockwise() {
	return new Counterclockwise(200);
}

MultAi *MAiMultCircleRole() {
	return new MultCircleRole;
}

bool MultCircleRole::Drive(Actor *actor) {
	auto point = Search(range_, actor, actor->goaltype_);
	if (point) {
		dynamic_cast<ModuleInstance *>(actor)->UseWeapon(true);
		/*auto it = members_.find(actor);
		if (it != members_.end()) {
		}*/
		members_.erase(actor);
		return true;
	} else {
		auto SetVel = [=](auto &ov) {
			d_vel pos = actor->GetCenterB2();
			d_vel v = master_->GetCenterB2();
			v -= pos;
			auto length = v.Normalize();
			if (length >= distance_max_) {
				ov = v;//ov应该是个单位向量，所以需要在乘以速度之前赋值
				v *= velocity_big;
				actor->SetVel(v);
				return false;
			} else if (length <= distance_min_) {
				ov = v;//ov应该是个单位向量，所以需要在乘以速度之前赋值
				v *= velocity_big;
				actor->SetVel(-v);
				return false;
			} else /*if (length <= distance_max_ && length >= distance_min_)*/ {
				for (auto it = members_.begin(); it != members_.end();) {
					if (!Exist(it->first)) {
						it = members_.erase(it);
					} else {
						it++;
					}
				}
				members_[actor] = nullptr;

				d_vel v1 = master_->GetCenterB2();
				b2Rot rt(-k2PI / n_);
				if (!ck_) {
					rt.Set(k2PI / n_);
				}
				auto vv = b2Mul(rt, v);
				vv *= dis_;
				v1 -= vv;
				d_vel xv(v1.x - actor->GetCenter().x, v1.y - actor->GetCenter().y);
				xv.Normalize();
				ov = xv;//ov应该是个单位向量，所以需要在乘以速度之前赋值
				xv *= velocity_small;
				auto n = members_.size();
				if (n <= 1) {
					actor->SetVel(xv);
					return true;
				}
				//前面先算出速度，这里找到最近的另一个僚机，如果本僚机在这个僚机的前面，本僚机的速度乘以2
				d_vel xdt(distance, distance);
				for (auto &it : members_) {
					if (it.first == actor) {
						continue;
					}
					auto xvv = pos - it.first->GetCenterB2();
					if (xvv.Length() < xdt.Length()) {
						xdt = xvv;
					}
				}

				auto cosangle = b2Dot(xdt, xv) / (xdt.Length() * xv.Length());
				if (cosangle > 0) {
					//本僚机的速度乘以2
					xv *= 2;
				} 
				actor->SetVel(xv);
				return true;
			}
			/*return true;*/
		};
		d_vel v;
		auto up = SetVel(v);
		if (up) {
			auto uv = d_vel(0, 1);
			actor->SetDirect(uv);
			SetAngle(actor, uv);
			dynamic_cast<ModuleInstance *>(actor)->UseWeapon(true);
		} else {
			actor->SetDirect(v);
			SetAngle(actor, v);
			dynamic_cast<ModuleInstance *>(actor)->UseWeapon(false);
		}
	}
	return false;
}

};
